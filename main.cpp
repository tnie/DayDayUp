#include "mainwindow.h"
#include "myclass.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    test();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
