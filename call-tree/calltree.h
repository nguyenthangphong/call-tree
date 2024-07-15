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
    ct_flag_t getFlag(QString flag);
    ct_mode_t getMode(QString mode);
    void build();
    void run();

private slots:
    void on_browserButton_clicked();
    void on_executeButton_clicked();

private:
    Ui::CallTree *ui;
    QString m_path;
    QString m_option;
    ct_mode_t m_mode;
    ct_flag_t m_flag;
    QString m_directory;
    QString m_file;
};
#endif // CALLTREE_H
