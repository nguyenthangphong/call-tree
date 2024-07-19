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
    ct_flag_t get_flag(const QString flag);
    ct_mode_t get_mode(const QString mode);
    ct_status_t build(const QString path, const QString textPath, const QString option, QString directoryPath, QString filePath, QProcess *process);
    ct_status_t run(const ct_flag_t flag, const QString path);
    ct_status_t run_su_file(const QString su_file);
    ct_status_t run_rtl_expand_file(const QString rtl_expand_path);
    QMap<QString, ct_function_data_t> get_function_name(QFile *file);
    ct_status_t add_call_tree(const QMap<QString, ct_function_data_t> input, const QList<QString> list_target_functions, QMap<int, QMap<QString, QMap<QString, QString>>> output);
    ct_status_t get_call_tree(); 

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
