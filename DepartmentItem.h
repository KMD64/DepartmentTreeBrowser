#ifndef DEPARTMENTITEM_H
#define DEPARTMENTITEM_H

#include "EmploymentItem.h"
#include "TreeItem.h"
#include <QObject>
#include <QVector>


class DepartmentItem:public TreeItem{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qint32 employeeCount READ employeeCount)
    Q_PROPERTY(qreal avgSalary READ avgSalary)
public:
    explicit DepartmentItem(QObject* parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    qint32 employeeCount();
    qreal avgSalary();

private:
    QString _name;
};
#endif // DEPARTMENTITEM_H
