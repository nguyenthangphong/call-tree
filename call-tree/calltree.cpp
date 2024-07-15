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

#ifdef Q_OS_LINUX
    process->setWorkingDirectory(dir->absolutePath());
    process->startDetached("/bin/sh", arguments);
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
    // switch (m_flag) {
    // case FSTACK_USAGE:
    //     break;
    // case FDUMP_RTL_EXPAND:
    //     break;
    // default:
    //     break;
    // }

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    ui->textEdit->setText(in.readAll());
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
