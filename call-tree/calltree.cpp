#include "calltree.h"
#include "ui_calltree.h"
#include <QDebug>
#include <QProcess>
#include <QDir>

CallTree::CallTree(QWidget *parent) : QMainWindow(parent), ui(new Ui::CallTree)
{
    ui->setupUi(this);
}

CallTree::~CallTree()
{
    delete ui;
}

void CallTree::on_browserButton_clicked()
{
    m_path = QFileDialog::getOpenFileName(this, "Open File");
    if (m_path.isEmpty()) return;
    ui->fileNameLineEdit->setText(m_path);
}

void CallTree::on_runButton_clicked()
{
    QString currentText = ui->comboBox->currentText();
    QProcess compilerProcess;
    QStringList arguments;
    QString scriptPath;

    QDir directory(QDir::currentPath());

    if (currentText == "Stack Size (-fstack-usage)") {
        m_option = "-fstack-usage";
    }

    if (currentText == "RTL Expand (-fdump-rtl-expand)") {
        m_option = "-fdump-rtl-expand";
    }
#ifdef Q_OS_LINUX
    directory.cdUp();
    scriptPath = directory.absoluteFilePath("compiler.sh");
    arguments << scriptPath << m_path << m_option;
    compilerProcess.startDetached("/bin/sh", arguments);
#else
    directory.cdUp();
    directory.cdUp();
    scriptPath = directory.absoluteFilePath("compiler.bat");
    arguments << "/C" << scriptPath << m_path << m_option;
    compilerProcess.startDetached("cmd.exe", arguments);
#endif
    if (!compilerProcess.waitForFinished()) {
        int result = compilerProcess.exitCode();

        if (result != 0) {
            QMessageBox::critical(nullptr, "Call Tree Compiler", "Compiler Fail...");
        } else {
            QMessageBox::information(nullptr, "Call Tree Compiler", "Compiler Done...");
        }
    } else {
        QMessageBox::critical(nullptr, "Call Tree Compiler", "Failed to start process");
    }
}
