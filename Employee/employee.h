#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QSharedData>
#include <QString>

class EmployeeData;

class Employee
{
  public:
    Employee();
    Employee(int id, const QString &name);
    Employee(const Employee &other);
    ~Employee();
    void setId(int id);
    void setName(const QString &name);

    int id() const;
    QString name() const;

  private:
    QSharedDataPointer<EmployeeData> d;
};

#endif // EMPLOYEE_H
