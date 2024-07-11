#include "calltree.h"
#include "ui_calltree.h"
#include <QDebug>
#include <QProcess>

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

    if (currentText == "Stack Size (-fstack-usage)") {
        m_option = "-fstack-usage";
    }

    if (currentText == "RTL Expand (-fdump-rtl-expand)") {
        m_option = "-fdump-rtl-expand";
    }
#ifdef Q_OS_LINUX
    arguments << "compiler.sh" << m_path << m_option;
    compilerProcess.startDetached("/bin/sh", arguments);
#else
    arguments << "compiler.bat" << m_path << m_option;
    compilerProcess.startDetached("cmd.exe", QStringList() << "/C" << arguments.join(" "));
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
