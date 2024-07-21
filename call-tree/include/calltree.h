#ifndef CALLTREE_H
#define CALLTREE_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include "call_tree_type.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CallTree; }
QT_END_NAMESPACE

class CallTree : public QMainWindow
{
    Q_OBJECT

public:
    CallTree(QWidget *parent = nullptr);
    ~CallTree();
    ct_flag_t getFlag(const QString flag);
    ct_mode_t getMode(const QString mode);
    ct_status_t buildCompiler(const QString filePath, const QString textPath, const QString option, QString directoryName, QString fileName, QProcess *process);
    ct_status_t runFile(const ct_flag_t flag, const QString filePath);
    ct_status_t runStackUsageFile(const QString stackUsageFile);
    ct_status_t runRTLExpandFile(const QString rtlExpandFile);
    QMap<QString, QStringList> getFunctionAndCall(QFile *file);

private slots:
    void on_browserButton_clicked();
    void on_executeButton_clicked();
    void on_txtBrowserButton_clicked();

private:
    Ui::CallTree *ui;

};
#endif // CALLTREE_H
