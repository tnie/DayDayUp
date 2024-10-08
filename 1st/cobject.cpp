#include "cobject.h"
#include <QVariant>
#include <QDebug>

//为什么需要另起文件？ moc 的局限吗？
//class DObject: public QObject
//{
//    Q_OBJECT
//public:
//    ~DObject(){};
//};

CObject::CObject(QObject *parent) : QObject(parent)
{
    this->setProperty("date", "2023年3月21日");
}

CObject::~CObject()
{
    qDebug() << __FUNCTION__;
}

int CObject::age() const
{
    return age_;
}

void CObject::setAge(int a)
{
    if(a != age_)
    {
        age_ = a;
        emit ageChanged(age_);
    }
//    auto p1 = new DObject;
//    qDebug() << p1->metaObject();
}

void CObject::on_objectNameChanged()
{
    // 可以在此处断点调试，观察信号槽的调用堆栈，观察
//    元对象系统如何调用 QObject 的特定函数
    setAge(0);
}

DObject::DObject(QObject *parent) : CObject(parent)
{

}

