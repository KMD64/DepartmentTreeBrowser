#ifndef DEPARTMENTITEM_H
#define DEPARTMENTITEM_H

#include "EmploymentItem.h"
#include "TreeItem.h"


class DepartmentItem:public TreeItem{
    Q_OBJECT
    Q_PROPERTY(qint32 employeeCount READ employeeCount)
    Q_PROPERTY(qreal avgSalary READ avgSalary)
public:
    explicit DepartmentItem(QObject* parent = nullptr);
    qint32 employeeCount();
    qreal avgSalary();

};
#endif // DEPARTMENTITEM_H
