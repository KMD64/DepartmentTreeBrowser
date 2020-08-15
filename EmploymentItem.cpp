#include "EmploymentItem.h"

EmploymentItem::EmploymentItem(QObject *parent) :
    TreeItem(parent),
    _function("Function")
{
    setName("Surname Name MiddleName");
}

QString EmploymentItem::function() const
{
    return _function;
}

void EmploymentItem::setFunction(const QString &function)
{
    _function = function;
}

qint32 EmploymentItem::salary() const
{
    return _salary;
}

void EmploymentItem::setSalary(const qint32 &salary)
{
    _salary = salary;
}


bool EmploymentItem::mapped()
{
    return false;
}


