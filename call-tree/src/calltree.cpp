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

ct_flag_t CallTree::get_flag(const QString flag)
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

ct_mode_t CallTree::get_mode(const QString mode)
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

ct_status_t CallTree::run_su_file(const QString su_file)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QRegularExpression re;
    QRegularExpressionMatch match;
    QFile file(su_file);

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

QMap<QString, ct_function_data_t> CallTree::get_function_name(QFile *file)
{
    QTextStream stream(file);
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QString function_name;
    QString target_name;
    QMap<QString, ct_function_data_t> functions;

    while (!stream.atEnd())
    {
        QString line = stream.readLine();

        regex.setPattern(REGEX_GET_FUNCTION_MAIN);
        match = regex.match(line);

        if (match.hasMatch())
        {
            function_name = match.captured("function");
            regex.setPattern(REGEX_GET_FUNCTION_EXCLUDE);
            match = regex.match(function_name);

            if (!functions.contains(function_name) && !match.hasMatch())
            {
                ct_function_data_t data;
                data.calls = QList<QString>();
                data.refs = QMap<QString, bool>();
                functions.insert(function_name, data);
            }
        }
        else
        {
            regex.setPattern(REGEX_GET_FUNCTION_CALLED);
            match = regex.match(line);

            if (match.hasMatch())
            {
                target_name = match.captured("target");

                if (target_name != "__stack_chk_fail")
                {
                    regex.setPattern(REGEX_GET_FUNCTION_EXCLUDE);
                    match = regex.match(target_name);

                    if (!match.hasMatch() && !functions[function_name].calls.contains(target_name))
                    {
                        functions[function_name].calls.append(target_name);
                    }
                }
                else
                {
                    regex.setPattern(REGEX_GET_FUNCTION_SYMBOL_REF);
                    match = regex.match(line);
                    
                    if (match.hasMatch())
                    {
                        target_name = match.captured("target");

                        if (functions[function_name].refs.contains(target_name))
                        {
                            functions[function_name].refs[target_name] = true;
                        }
                    }
                }
            }
        }
    }

    return functions;
}

// ct_status_t CallTree::add_call_tree(
//     const QMap<QString, ct_function_data_t> input, 
//     const QList<QString> list_target_functions, 
//     QMap<int, QMap<QString, QMap<QString, QString>>> output
// )
// {
//     ct_status_t ret = STATUS_OK;


//     return ret;
// }

// ct_status_t CallTree::get_call_tree()
// {

// }

ct_status_t CallTree::run_rtl_expand_file(const QString rtl_expand_path)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QFile file(rtl_expand_path);
    QMap<QString, ct_function_data_t> functions;

    if (!file.open(QIODevice::ReadOnly))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        functions = get_function_name(&file);

        // Show functions in GUI
        for (const QString function_main : functions.keys())
        {
            if (function_main.contains("R_IMPDRV"))
            {
                ui->resultTextEdit->append(function_main);
            }

            for (const ct_function_data_t functions_call : functions.values())
            {
                for (int i = 0; i < functions_call.calls.size(); i++)
                {
                    ui->resultTextEdit->append("|____" + functions_call.calls.at(i));
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

    m_flag = get_flag(textFlag);
    m_mode = get_mode(textMode);

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
