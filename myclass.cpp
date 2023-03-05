#include "myclass.h"
#include <QDebug>

MyClass::MyClass(QObject *parent) : QObject(parent)
{

}

MyClass::~MyClass()
{

}

void test()
{
    MyClass * c1 = new MyClass();
    c1->setPriority(MyClass::High);
    qDebug() << c1->priority();

    QObject * c2 = c1;
    qDebug() << c2->property("priority");
    c2->setProperty("priority", "VeryHigh");
    qDebug() << c2->property("priority");
    qDebug() << c1->priority();
}
