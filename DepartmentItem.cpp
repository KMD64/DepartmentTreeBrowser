#include "DepartmentItem.h"
DepartmentItem::DepartmentItem(QObject *parent) : TreeItem(parent)
{
    setName("Department");
}

qint32 DepartmentItem::employeeCount()
{
    return childItems().size();
}

qreal DepartmentItem::avgSalary()
{
    qint32 count = employeeCount();
    if(count==0){return 0;}

    qint64 sum = 0;
    for(TreeItem *item:childItems()){
        if(auto employee = dynamic_cast<EmploymentItem*>(item);employee){
            sum += employee->salary();
        }
    }

    return sum/static_cast<qreal>(count);
}

