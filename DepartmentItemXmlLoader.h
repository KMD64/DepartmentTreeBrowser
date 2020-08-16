#ifndef DEPARTMENTITEMXMLLOADER_H
#define DEPARTMENTITEMXMLLOADER_H

#include "DepartmentItem.h"

#include <qdom.h>



class DepartmentItemXmlLoader
{
    const QPointer<DepartmentItem> _item;
public:
    DepartmentItemXmlLoader(DepartmentItem *item);
    bool load(const QDomNode &node,QString *errmsg=nullptr);
    QDomNode save();
};

#endif // DEPARTMENTITEMXMLLOADER_H
