#ifndef EMPLOYMENTITEM_H
#define EMPLOYMENTITEM_H

#include "TreeItem.h"

class EmploymentItem : public TreeItem
{
    Q_OBJECT
    Q_PROPERTY(QString function READ function WRITE setFunction)
    Q_PROPERTY(qint32 salary READ salary WRITE setSalary)
public:
    explicit EmploymentItem(QObject *parent = nullptr);


    QString function() const;
    void setFunction(const QString &function);

    qint32 salary() const;
    void setSalary(const qint32 &salary);

private:
    QString _function;
    qint32 _salary=0;
    // TreeItem interface
public:
    bool mapped() override;
};

#endif // EMPLOYMENTITEM_H
