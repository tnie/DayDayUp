#include <QCoreApplication>
#include "employee.h"

//学习 QSharedDataPointer Class 手册，隐式共享/写时拷贝
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Employee e1(1001, "Albrecht Durer");
    Employee e2 = e1;
    QString n("Hans Holbein");
    e1.setName(n);
    e1.name();
    return a.exec();
}
