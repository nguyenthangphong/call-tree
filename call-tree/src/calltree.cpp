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

ct_flag_t CallTree::getFlag(const QString flag)
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

ct_mode_t CallTree::getMode(const QString mode)
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

ct_status_t CallTree::build(
    const QString path, const QString textPath, const QString option, QString directoryPath, QString filePath, QProcess *process
)
{
    ct_status_t ret = STATUS_OK;
    QStringList gccArguments;
    QRegularExpression re;
    QRegularExpressionMatch match;

    re.setPattern("^.*[\\\\/]");
    match = re.match(path);
    directoryPath = match.hasMatch() ? match.captured(0) : "";

    re.setPattern("[^\\\\/]+$");
    match = re.match(path);
    filePath = match.hasMatch() ? match.captured(0) : "";

#ifdef Q_OS_LINUX
    if (!textPath.isEmpty()) {
        QFileInfo fileInfo(directoryPath);
        QString parentDir = fileInfo.dir().path();
        QFile file(textPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            ret = STATUS_ERROR;

        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains("include")) gccArguments << "-I" + line;
            else {
                QString projectName = line;
                int projectIndex = parentDir.indexOf(projectName);

                if (projectIndex != -1) {
                    QString basePath = parentDir.left(projectIndex);
                    QString suffix = directoryPath.mid(basePath.length() + 1 + projectName.length());
                    directoryPath = basePath + projectName;
                    filePath = suffix + filePath;
                    gccArguments << "-c" << filePath;
                }
            }
        }

        gccArguments << option;
    } else {
        gccArguments << "-c" << filePath << option;
    }

    process->setWorkingDirectory(directoryPath);
    process->start("gcc", gccArguments);
#elif defined(Q_OS_WIN)
    re.setPattern("^.*[\\\\/]");
    match = re.match(path);
    directoryPath = match.hasMatch() ? match.captured(0) : "";

    re.setPattern("[^\\\\/]+$");
    match = re.match(path);
    filePath = match.hasMatch() ? match.captured(0) : "";

    gccArguments << "-c" << filePath << option;

    process->setWorkingDirectory(directoryPath);
    process->start("gcc", gccArguments);
#endif
    return ret;
}

ct_status_t CallTree::run(const ct_flag_t flag, const QString path)
{
    ct_status_t ret;

    switch (flag) {
    case FSTACK_USAGE:
        ret = run_su_file(path);
        break;
    case FDUMP_RTL_EXPAND:
        ret = run_rtl_expand_file(path);
        break;
    default:
        ret = STATUS_ERROR;
        break;
    }

    return ret;
}

ct_status_t CallTree::run_su_file(const QString path)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QRegularExpression re;
    QRegularExpressionMatch match;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        ret = STATUS_ERROR;

    QTextStream in(&file);

    while (!in.atEnd()) {
        re.setPattern(":(\\w+)\\t(\\d+)\\t");
        QString line = in.readLine();
        match = re.match(line);

        if (match.hasMatch()) {
            ui->resultTextEdit->append(match.captured(1) + " " + match.captured(2));
        }
    }

    return ret;
}

ct_status_t CallTree::run_rtl_expand_file(const QString path)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QFile file(path);
    QMap<QString, ct_function_data_t> functions;

    if (!file.open(QIODevice::ReadOnly))
        ret = STATUS_ERROR;

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

    return ret;
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
    ct_status_t ret;
    QString textFlag = ui->flagComboBox->currentText();
    QString textMode = ui->modeComboBox->currentText();
    QProcess process;

    m_flag = getFlag(textFlag);
    m_mode = getMode(textMode);

    if (m_flag == FSTACK_USAGE) {
        m_option = "-fstack-usage";
    }

    if (m_flag == FDUMP_RTL_EXPAND) {
        m_option = "-fdump-rtl-expand";
    }

    switch (m_mode) {
    case BUILD:
        {
            ret = build(m_path, m_txt, m_option, m_directory, m_file, &process);

            if (!process.waitForStarted()) {
                QMessageBox::critical(nullptr, "Call Tree", "Failed to start process...");
            }

            if (!process.waitForFinished()) {
                QMessageBox::critical(nullptr, "Call Tree", "Process did not finish...");
            }

            int result = process.exitCode();

            if (result != 0) {
                QMessageBox::critical(nullptr, "Call Tree", "Compilation failed...");
            } else {
                QMessageBox::information(nullptr, "Call Tree", "Compilation done...");
            }
        }
        break;
    case RUN:
        ret = run(m_flag, m_path);
        break;
    default:
        ret = STATUS_ERROR;
        break;
    }

    qDebug() << "ret = " << ret;
}
