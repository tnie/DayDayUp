#include "mainwindow.h"

#include <QApplication>

//shijz-report 摸索 Qt 的数据库操作
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
