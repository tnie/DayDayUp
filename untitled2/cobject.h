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
    void on_objectNameChanged();

signals:
    void ageChanged(int);

private:
    int age_;

};

#endif // COBJECT_H
