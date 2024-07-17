#include "include/calltree.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CallTree w;
    w.show();
    return a.exec();
}
