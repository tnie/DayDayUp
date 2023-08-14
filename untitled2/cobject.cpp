#include "cobject.h"
#include <QVariant>

CObject::CObject(QObject *parent) : QObject(parent)
{
    this->setProperty("date", "2023年3月21日");
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
}

void CObject::on_objectNameChanged()
{
    setAge(0);
}
