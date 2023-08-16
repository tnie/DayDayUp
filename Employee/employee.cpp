#include "employee.h"

class EmployeeData : public QSharedData
{
  public:
    EmployeeData() : id(-1) { }
    EmployeeData(const EmployeeData &other)
        : QSharedData(other), id(other.id), name(other.name) { }
    ~EmployeeData() { }

    int id;
    QString name;
};

Employee::Employee() { d = new EmployeeData; }

Employee::Employee(int id, const QString &name) {
    d = new EmployeeData;
    setId(id);
    setName(name);
}

Employee::Employee(const Employee &other)
    : d (other.d)
{
}

Employee::~Employee()
{

}

void Employee::setId(int id) { d->id = id; }

void Employee::setName(const QString &name) { d->name = name; }

int Employee::id() const { return d->id; }

QString Employee::name() const { return d->name; }
