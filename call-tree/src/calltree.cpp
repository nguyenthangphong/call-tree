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
    ct_flag_t ret;

    if (flag == "Stack Size")
    {
        ret = FSTACK_USAGE;
    }
    else if (flag == "RTL Expand")
    {
        ret = FDUMP_RTL_EXPAND;
    }
    else
    {
        ret = FLAG_ERROR;
    }

    return ret;
}

ct_mode_t CallTree::getMode(const QString mode)
{
    ct_mode_t ret;

    if (mode == "Build")
    {
        ret = BUILD;
    }
    else if (mode == "Run")
    {
        ret = RUN;
    }
    else
    {
        ret = MODE_ERROR;
    }

    return ret;
}

ct_status_t CallTree::buildCompiler(
    const QString filePath, const QString textPath, const QString option, QString directoryName, QString fileName, QProcess *process
)
{
    ct_status_t ret = STATUS_OK;
    QStringList gccArguments;
    QRegularExpression regex;
    QRegularExpressionMatch match;

    regex.setPattern("^.*[\\\\/]");
    match = regex.match(filePath);
    directoryName = match.hasMatch() ? match.captured(0) : "";

    regex.setPattern("[^\\\\/]+$");
    match = regex.match(filePath);
    fileName = match.hasMatch() ? match.captured(0) : "";

    if (!textPath.isEmpty())
    {
        QFileInfo fileInfo(directoryName);
        QString parentDir = fileInfo.dir().path();
        QFile file(textPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ret = STATUS_ERROR;
        }

        QTextStream in(&file);

        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.contains("include"))
            {
                gccArguments << "-I" + line;
            }
            else
            {
                QString projectName = line;
                int projectIndex = parentDir.indexOf(projectName);

                if (projectIndex != -1)
                {
                    QString basePath = parentDir.left(projectIndex);
                    QString suffix = directoryName.mid(basePath.length() + 1 + projectName.length());
                    directoryName = basePath + projectName;
                    fileName = suffix + filePath;
                    gccArguments << "-c" << filePath;
                }
            }
        }

        gccArguments << option;
    }
    else
    {
        gccArguments << "-c" << filePath << option;
    }

    process->setWorkingDirectory(directoryName);
    process->start("gcc", gccArguments);

    return ret;
}

ct_status_t CallTree::runFile(const ct_flag_t flag, const QString filePath)
{
    ct_status_t ret;

    switch (flag)
    {
    case FSTACK_USAGE:
        ret = runStackUsageFile(filePath);
        break;
    case FDUMP_RTL_EXPAND:
        ret = runRTLExpandFile(filePath);
        break;
    default:
        ret = STATUS_ERROR;
        break;
    }

    return ret;
}

ct_status_t CallTree::runStackUsageFile(const QString stackUsageFile)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QFile file(stackUsageFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ret = STATUS_ERROR;
    }

    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        QString line = stream.readLine();

        regex.setPattern(":(\\w+)\\t(\\d+)\\t");
        match = regex.match(line);

        if (match.hasMatch())
        {
            ui->resultTextEdit->append(match.captured(1) + " " + match.captured(2));
        }
    }

    return ret;
}

QMap<QString, QStringList> CallTree::getFunctionAndCall(QFile *file)
{
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QString functionName;
    QString targetName;
    QMap<QString, QStringList> functionMainAndListFunctionCall;
    QTextStream stream(file);

    while (!stream.atEnd())
    {
        QString line = stream.readLine();

        regex.setPattern("^;; Function (?P<mangle>.*)\\s+\\((?P<function>\\S+)(,.*)?\\).*$");
        match = regex.match(line);

        if (match.hasMatch())
        {
            functionName = match.captured("function");
            regex.setPattern("R_OSAL|memcpy");
            match = regex.match(functionName);

            if (!functionMainAndListFunctionCall.contains(functionName) && !match.hasMatch())
            {
                functionMainAndListFunctionCall.insert(functionName, QStringList());
            }
        }
        else
        {
            regex.setPattern("^.*\\(call.*\"(?P<target>.*)\".*$");
            match = regex.match(line);

            if (match.hasMatch())
            {
                targetName = match.captured("target");

                if (targetName != "__stack_chk_fail")
                {
                    regex.setPattern("R_OSAL|memcpy");
                    match = regex.match(targetName);

                    if (!match.hasMatch() && !functionMainAndListFunctionCall[functionName].contains(targetName))
                    {
                        functionMainAndListFunctionCall[functionName].append(targetName);
                    }
                }
            }
        }
    }

    return functionMainAndListFunctionCall;
}

ct_status_t CallTree::runRTLExpandFile(const QString rtlExpandFile)
{
    // Clear data before read other file
    ui->resultTextEdit->setText("");

    ct_status_t ret = STATUS_OK;
    QFile file(rtlExpandFile);
    QMap<QString, QStringList> functionMainAndListFunctionCall;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        functionMainAndListFunctionCall = getFunctionAndCall(&file);

        // Show functions in GUI
        for (auto index = functionMainAndListFunctionCall.begin(); index != functionMainAndListFunctionCall.end(); ++index)
        {
            QString functionMain = index.key();
            QStringList listFunctionCall = index.value();

            ui->resultTextEdit->append("Function main: " + functionMain);

            for (const QString &functionCall : listFunctionCall)
            {
                ui->resultTextEdit->append(functionCall + " called by " + functionMain);
            }

            ui->resultTextEdit->append("----------------------------------");
        }
    }

    return ret;
}

void CallTree::on_browserButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File");
    if (filePath.isEmpty()) return;
    ui->fileNameLineEdit->setText(filePath);
}

void CallTree::on_txtBrowserButton_clicked()
{
    QString textPath = QFileDialog::getOpenFileName(this, "Open File");
    if (textPath.isEmpty()) return;
    ui->txtLineEdit->setText(textPath);
}

void CallTree::on_executeButton_clicked()
{
    ct_status_t ret = STATUS_OK;
    QString textFlag, textMode, filePath, textPath, directoryName, fileName, option;
    QProcess process;
    ct_flag_t flag;
    ct_mode_t mode;

    filePath = ui->fileNameLineEdit->text();
    textPath = ui->txtLineEdit->text();

    textFlag = ui->flagComboBox->currentText();
    flag = getFlag(textFlag);

    if (flag == FSTACK_USAGE)
    {
        option = "-fstack-usage";
    }

    if (flag == FDUMP_RTL_EXPAND)
    {
        option = "-fdump-rtl-expand";
    }

    textMode = ui->modeComboBox->currentText();
    mode = getMode(textMode);

    switch (mode)
    {
    case BUILD:
        {
            ret = buildCompiler(filePath, textPath, option, directoryName, fileName, &process);

            if (!process.waitForStarted())
            {
                QMessageBox::critical(nullptr, "Call Tree", "Failed to start process...");
            }

            if (!process.waitForFinished())
            {
                QMessageBox::critical(nullptr, "Call Tree", "Process did not finish...");
            }

            int result = process.exitCode();

            if (result != 0)
            {
                QMessageBox::critical(nullptr, "Call Tree", "Compilation failed...");
            }
            else
            {
                QMessageBox::information(nullptr, "Call Tree", "Compilation done...");
            }
        }
        break;
    case RUN:
        ret = runFile(flag, filePath);
        break;
    default:
        ret = STATUS_ERROR;
        break;
    }

    qDebug() << "ret = " << ret;
}
