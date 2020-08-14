#ifndef EMPLOYMENTITEM_H
#define EMPLOYMENTITEM_H

#include "TreeItem.h"

class EmploymentItem : public TreeItem
{
    Q_OBJECT
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName)
    Q_PROPERTY(QString function READ function WRITE setFunction)
    Q_PROPERTY(qint32 salary READ salary WRITE setSalary)
public:
    explicit EmploymentItem(QObject *parent = nullptr);
    QString surname() const;
    void setSurname(const QString &surname);

    QString name() const;
    void setName(const QString &name);

    QString middleName() const;
    void setMiddleName(const QString &middleName);

    QString function() const;
    void setFunction(const QString &function);

    qint32 salary() const;
    void setSalary(const qint32 &salary);

    QString fullName();
    void setFullName(const QString&);


private:
    QString _surname;
    QString _name;
    QString _middleName;
    QString _function;
    qint32 _salary=0;
    void resetFullName();

    // TreeItem interface
public:
    bool mapped() override;
};

#endif // EMPLOYMENTITEM_H
