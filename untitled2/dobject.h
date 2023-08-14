#ifndef DOBJECT_H
#define DOBJECT_H

#include "cobject.h"

class DObject: public CObject
{
    Q_OBJECT
public:
    explicit DObject(QObject *parent = nullptr);

signals:
};


#endif // DOBJECT_H
