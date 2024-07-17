#include "calltree.h"
#include "ui_calltree.h"
#include <QRegularExpression>
#include <QFile>

CallTree::CallTree(QWidget *parent) : QMainWindow(parent), ui(new Ui::CallTree)
{
    ui->setupUi(this);
}

CallTree::~CallTree()
{
    delete ui;
}

ct_flag_t CallTree::getFlag(QString flag)
{
    ct_flag_t res;

    if (flag == "Stack Size") {
        res = FSTACK_USAGE;
    } else if (flag == "RTL Expand") {
        res = FDUMP_RTL_EXPAND;
    } else {
        res = FLAG_ERROR;
    }

    return res;
}

ct_mode_t CallTree::getMode(QString mode)
{
    ct_mode_t res;

    if (mode == "Build") {
        res = BUILD;
    } else if (mode == "Run") {
        res = RUN;
    } else {
        res = MODE_ERROR;
    }

    return res;
}

void CallTree::build()
{
    QStringList gccArguments;
    QProcess process;
    QRegularExpression re;
    QRegularExpressionMatch match;
    QString txtPath =  ui->txtLineEdit->text();

    re.setPattern("^.*[\\\\/]");
    match = re.match(m_path);
    m_directory = match.hasMatch() ? match.captured(0) : "";

    re.setPattern("[^\\\\/]+$");
    match = re.match(m_path);
    m_file = match.hasMatch() ? match.captured(0) : "";

#ifdef Q_OS_LINUX
    if (!txtPath.isEmpty()) {
        QFileInfo fileInfo(m_directory);
        QString parentDir = fileInfo.dir().path();
        QFile file(txtPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains("include")) gccArguments << "-I" + line;
            else {
                QString projectName = line;
                int projectIndex = parentDir.indexOf(projectName);

                if (projectIndex != -1) {
                    QString basePath = parentDir.left(projectIndex);
                    QString suffix = m_directory.mid(basePath.length() + 1 + projectName.length());
                    m_directory = basePath + projectName;
                    qDebug() << "m_directory = " << m_directory;
                    m_file = suffix + m_file;
                    qDebug() << "m_file = " << m_file;
                    gccArguments << "-c" << m_file;
                }
            }
        }

        gccArguments << m_option;
    } else {
        gccArguments << "-c" << m_file << m_option;
    }

    process.setWorkingDirectory(m_directory);
    process.start("gcc", gccArguments);
#elif defined(Q_OS_WIN)
    re.setPattern("^.*[\\\\/]");
    match = re.match(m_path);
    m_directory = match.hasMatch() ? match.captured(0) : "";

    re.setPattern("[^\\\\/]+$");
    match = re.match(m_path);
    m_file = match.hasMatch() ? match.captured(0) : "";

    gccArguments << "-c" << m_file << m_option;

    process.setWorkingDirectory(m_directory);
    process.start("gcc", gccArguments);
#endif
    if (!process.waitForStarted()) {
        QMessageBox::critical(nullptr, "Call Tree", "Failed to start process...");
        return;
    }

    if (!process.waitForFinished()) {
        QMessageBox::critical(nullptr, "Call Tree", "Process did not finish...");
        return;
    }

    int result = process.exitCode();

    if (result != 0) {
        QMessageBox::critical(nullptr, "Call Tree", "Compilation failed...");
    } else {
        QMessageBox::information(nullptr, "Call Tree", "Compilation done...");
    }
}

void CallTree::run()
{
    switch (m_flag) {
    case FSTACK_USAGE:
        run_su_file();
        break;
    case FDUMP_RTL_EXPAND:
        run_rtl_expand_file();
        break;
    default:
        break;
    }
}

void CallTree::run_su_file()
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    QRegularExpression re;
    QRegularExpressionMatch match;
    QFile file(m_path);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    while (!in.atEnd()) {
        re.setPattern(":(\\w+)\\t(\\d+)\\t");
        QString line = in.readLine();
        match = re.match(line);

        if (match.hasMatch()) {
            ui->resultTextEdit->append(match.captured(1) + " " + match.captured(2));
        }
    }
}

void CallTree::run_rtl_expand_file()
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    QFile file(m_path);
    QMap<QString, ct_function_data_t> functions;

    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    QRegularExpression function("^;; Function (?P<mangle>.*)\\s+\\((?P<function>\\S+)(,.*)?\\).*$");
    QRegularExpression call("^.*\\(call.*\"(?P<target>.*)\".*$");
    QRegularExpression symbol_ref("^.*\\(symbol_ref.*\"(?P<target>.*)\".*$");
    QRegularExpression exclude("R_OSAL|memcpy");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = function.match(line);
        QString functionName = "";
        QString target = "";

        if (match.hasMatch()) {
            functionName = match.captured("function");
            ui->resultTextEdit->append(functionName);

            if (!functions.contains(functionName) && !exclude.match(functionName).hasMatch()) {
                ct_function_data_t data;
                data.calls = QList<QString>();
                data.refs = QMap<QString, bool>();
                functions.insert(functionName, data);
            }
        } else {
            match = call.match(line);

            if (match.hasMatch()) {
                target = match.captured("target");

                if (target != "__stack_chk_fail") {
                    if (!exclude.match(target).hasMatch() && !functions[functionName].calls.contains(target)) {
                        ui->resultTextEdit->append("   |_____" + target);
                        functions[functionName].calls.append(target);
                    }
                }
            } else {
                match = symbol_ref.match(line);
                
                if (match.hasMatch()) {
                    target = match.captured("target");
                    if (functions[functionName].refs.contains(target)) {
                        functions[functionName].refs[target] = true;
                    }
                }
            }
        }
    }
}

void CallTree::on_browserButton_clicked()
{
    m_path = QFileDialog::getOpenFileName(this, "Open File");
    if (m_path.isEmpty()) return;
    ui->fileNameLineEdit->setText(m_path);
}

void CallTree::on_txtBrowserButton_clicked()
{
    m_txt = QFileDialog::getOpenFileName(this, "Open File");
    if (m_txt.isEmpty()) return;
    ui->txtLineEdit->setText(m_txt);
}

void CallTree::on_executeButton_clicked()
{
    ct_status_t res;

    m_mode = getMode(ui->modeComboBox->currentText());
    m_flag = getFlag(ui->flagComboBox->currentText());

    if (m_flag == FSTACK_USAGE) {
        m_option = "-fstack-usage";
    }

    if (m_flag == FDUMP_RTL_EXPAND) {
        m_option = "-fdump-rtl-expand";
    }

    switch (m_mode) {
    case BUILD:
        build();
        break;
    case RUN:
        run();
        break;
    default:
        break;
    }
}
