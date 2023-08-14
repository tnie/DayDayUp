#include "mainwindow.h"
#include "myclass.h"
#include <QApplication>
#include <QtMath>

int main(int argc, char *argv[])
{
    qRadiansToDegrees(1.2);
    test();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
