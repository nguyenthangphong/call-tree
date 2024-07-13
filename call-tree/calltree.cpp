#include "calltree.h"
#include "ui_calltree.h"
#include <QDebug>

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
    } else if (mode == "Clean") {
        res = CLEAN;
    } else {
        res = MODE_ERROR;
    }

    return res;
}

bool CallTree::compiler_build(QDir *dir, QProcess *process)
{
    QStringList arguments;
    QString scriptPath;

    dir->setCurrent("call-tree/call-tree");

#ifdef Q_OS_LINUX
    if (dir->absolutePath().contains("build")) dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_build.sh");
    arguments << scriptPath << m_path << m_option;
    process->setWorkingDirectory(dir->absolutePath());
    process->startDetached("/bin/sh", arguments);
#else
    dir->cdUp();
    dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_build.bat");
    arguments << "/C" << scriptPath << m_path << m_option;
    process->startDetached("cmd.exe", arguments);
#endif
    return process->waitForFinished();
}

bool CallTree::compiler_run(QDir *dir, QProcess *process)
{
    QStringList arguments;
    QString scriptPath;

    dir->setCurrent("call-tree/call-tree");

#ifdef Q_OS_LINUX
    if (dir->absolutePath().contains("build")) dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_run.sh");
    arguments << scriptPath << m_path << m_option;
    process->setWorkingDirectory(dir->absolutePath());
    process->startDetached("/bin/sh", arguments);
#else
    dir->cdUp();
    dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_run.bat");
    arguments << "/C" << scriptPath << m_path << m_option;
    process->startDetached("cmd.exe", arguments);
#endif
    return process->waitForFinished();
}

bool CallTree::compiler_clean(QDir *dir, QProcess *process)
{
    QStringList arguments;
    QString scriptPath;

    dir->setCurrent("call-tree/call-tree");

#ifdef Q_OS_LINUX
    if (dir->absolutePath().contains("build")) dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_clean.sh");
    arguments << scriptPath << m_option;
    process->setWorkingDirectory(dir->absolutePath());
    process->startDetached("/bin/sh", arguments);
#else
    dir->cdUp();
    dir->cdUp();
    scriptPath = dir->absoluteFilePath("compiler_clean.bat");
    arguments << "/C" << scriptPath << m_option;
    process->startDetached("cmd.exe", arguments);
#endif
    return process->waitForFinished();
}

ct_status_t CallTree::build()
{
    QProcess compilerProcess;
    QDir directory;
    ct_status_t res;

    bool is_build_finished = compiler_build(&directory, &compilerProcess);

    if (!is_build_finished) {
        int result = compilerProcess.exitCode();

        if (result != 0) {
            res = STATUS_FAIL;
            QMessageBox::critical(this, "Call Tree Build", "Build Fail...");
        } else {
            res = STATUS_OK;
            QMessageBox::information(this, "Call Tree Build", "Build Done...");
        }
    } else {
        res = STATUS_ERROR;
        QMessageBox::critical(nullptr, "Call Tree Build", "Failed to start process");
    }

    return res;
}

ct_status_t CallTree::run()
{
    QProcess compilerProcess;
    QDir directory;
    ct_status_t res;

    bool is_run_finished = compiler_run(&directory, &compilerProcess);

    if (!is_run_finished) {
        int result = compilerProcess.exitCode();

        if (result != 0) {
            res = STATUS_FAIL;
        } else {
            res = STATUS_OK;
        }
    } else {
        res = STATUS_ERROR;
    }

    return res;
}

ct_status_t CallTree::clean()
{
    QProcess compilerProcess;
    QDir directory;
    ct_status_t res;

    bool is_clean_finished = compiler_clean(&directory, &compilerProcess);

    if (!is_clean_finished) {
        int result = compilerProcess.exitCode();

        if (result != 0) {
            res = STATUS_FAIL;
            QMessageBox::critical(this, "Call Tree Clean", "Clean Fail...");
        } else {
            res = STATUS_OK;
            QMessageBox::information(this, "Call Tree Clean", "Clean Done...");
        }
    } else {
        res = STATUS_ERROR;
        QMessageBox::critical(nullptr, "Call Tree Clean", "Failed to start process");
    }

    return res;
}

void CallTree::on_browserButton_clicked()
{
    m_path = QFileDialog::getOpenFileName(this, "Open File");
    if (m_path.isEmpty()) return;
    ui->fileNameLineEdit->setText(m_path);
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

    if (m_flag == FSTACK_USAGE) {
        switch (m_mode) {
        case BUILD:
            res = build();
            break;
        case RUN:
            res = run();
            break;
        case CLEAN:
            res = clean();
            break;
        default:
            res = STATUS_ERROR;
            break;
        }
    }

    qDebug() << "res = " << res;
}
