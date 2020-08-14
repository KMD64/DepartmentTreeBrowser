#include "EmploymentItem.h"

EmploymentItem::EmploymentItem(QObject *parent) :
    TreeItem(parent),
    _surname("Surname"),
    _name("Name"),
    _middleName("MiddleName"),
    _function("Function")
{
}

QString EmploymentItem::surname() const
{
    return _surname;
}

void EmploymentItem::setSurname(const QString &surname)
{
    _surname = surname;
}

QString EmploymentItem::name() const
{
    return _name;
}

void EmploymentItem::setName(const QString &name)
{
    _name = name;
}

QString EmploymentItem::middleName() const
{
    return _middleName;
}

void EmploymentItem::setMiddleName(const QString &middleName)
{
    _middleName = middleName;
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

QString EmploymentItem::fullName()
{
    return QString("%1 %2 %3").arg(_surname,_name,_middleName);
}

void EmploymentItem::setFullName(const QString& nFullName)
{
    resetFullName();
    auto names = nFullName.split(" ",QString::SplitBehavior::SkipEmptyParts);
    for(int i=0;i<names.size();++i){
        switch(i){
        case 0:{_surname=names[i];break;}
        case 1:{_name=names[i];break;}
        case 2:{_middleName=names[i];break;}
        }
        if(i>2)break;
    }
}

void EmploymentItem::resetFullName()
{
    _name="";
    _surname="";
    _middleName="";
}

bool EmploymentItem::mapped()
{
    return false;
}


