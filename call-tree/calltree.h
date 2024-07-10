#ifndef CALLTREE_H
#define CALLTREE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CallTree; }
QT_END_NAMESPACE

class CallTree : public QMainWindow
{
    Q_OBJECT

public:
    CallTree(QWidget *parent = nullptr);
    ~CallTree();

private:
    Ui::CallTree *ui;
};
#endif // CALLTREE_H
