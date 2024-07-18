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
    ct_status_t build(const QString path, const QString textPath, const QString option, QString directoryPath, QString filePath, QProcess *process);
    ct_status_t run(const ct_flag_t flag, const QString path);
    ct_status_t run_su_file(const QString path);
    ct_status_t run_rtl_expand_file(const QString path);

private slots:
    void on_browserButton_clicked();
    void on_executeButton_clicked();
    void on_txtBrowserButton_clicked();

private:
    Ui::CallTree *ui;
    QString m_path;
    QString m_option;
    ct_mode_t m_mode;
    ct_flag_t m_flag;
    QString m_directory;
    QString m_file;
    QString m_txt;
};
#endif // CALLTREE_H
