#include "test.h"
#include "cobject.h"
#include <QPointer>
#include <QDebug>
#include <QMetaMethod>

void moc::test1()
{
    QPointer<QObject> p1 = new QObject;
    const QString objectName = p1->objectName();
    qDebug() << objectName;
}

void moc::test2()
{
    CObject *o=new CObject();
    QMetaObject::Connection conn = QObject::connect(o, SIGNAL(objectNameChanged(const QString &)), o, SLOT(on_objectNameChanged(const QString &)));
    Q_ASSERT(conn);
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
}

#include "workerthread.h"
void thread::test()
{
    auto p = new MyObject;
    p->startWorkInAThread();
}
