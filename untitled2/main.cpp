#include <QCoreApplication>
#include <QDebug>
#include "cobject.h"
#include <QMetaMethod>
#include <QVariant>
#include <QPointer>

//为什么需要另起文件？ moc 的局限吗？
//class CObject: public QObject
//{
//    Q_OBJECT
//public:
//    ~CObject(){};
//};

void test1()
{
    QPointer<QObject> p1 = new QObject;
    const QString objectName = p1->objectName();
    qDebug() << objectName;
}

int main(int argc, char *argv[])
{
    test1();
    QCoreApplication a(argc, argv);
    CObject *o=new CObject();
//    QObject::connect(o, SIGNAL(objectNameChanged), o, SLOT(on_objectNameChanged));
    o->setObjectName("name1");
//    对象的元信息
    const QMetaObject *m=o->metaObject();
    const char *className = m->className();
    if(int c = m->constructorCount())
    {
        qDebug() << "\n" << className << "constructorCount" << c;
        for(int i=0; i<c; ++i)
        {
    //        成员函数的元信息
            QMetaMethod m1 = m->constructor(i);
            qDebug() << m1.name();
        }
    }
    if(int c = m->enumeratorCount())
    {
        qDebug() << "\n" << className << "enumeratorCount" << c;
        for(int i=0; i<c; ++i)
        {
    //        枚举的元信息
            QMetaEnum m1 = m->enumerator(i);
            qDebug() << m1.name();
        }
    }
    if(int c = m->propertyCount())
    {
//        不包含动态属性！
        qDebug() << "\n" << className << "propertyCount" << c;
        for(int i=0; i<c; ++i)
        {
    //        属性的元信息
            QMetaProperty m1 = m->property(i);
            const char *n = m1.name();
            QVariant var = m1.read(o);  /*how?*/
            qDebug() << n << var;
        }
    }
    if(int c = m->methodCount())
    {
        qDebug() << "\n" << className << "methodCount" << c;
        for(int i=0; i<c; ++i)
        {
    //        成员函数的元信息
            QMetaMethod m1 = m->method(i);
            qDebug() << m1.methodSignature();
        }
    }

    qDebug() << "\n";
    o->dumpObjectInfo();

    o->setAge(11);

    return a.exec();
}
