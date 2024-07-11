#ifndef CALLTREE_H
#define CALLTREE_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <cstdlib>

QT_BEGIN_NAMESPACE
namespace Ui { class CallTree; }
QT_END_NAMESPACE

class CallTree : public QMainWindow
{
    Q_OBJECT

public:
    CallTree(QWidget *parent = nullptr);
    ~CallTree();

private slots:
    void on_browserButton_clicked();
    void on_runButton_clicked();

private:
    Ui::CallTree *ui;
    QString m_path;
    QString m_option;
};
#endif // CALLTREE_H
