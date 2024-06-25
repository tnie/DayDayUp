#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>

class CObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)
public:
    explicit CObject(QObject *parent = nullptr);
    int age() const;
    void setAge(int a);

public slots:
    void on_objectNameChanged();

signals:
    void ageChanged(int);

private:
    int age_;

};

class DObject: public CObject
{
    Q_OBJECT
public:
    explicit DObject(QObject *parent = nullptr);

signals:
};

#endif // COBJECT_H
