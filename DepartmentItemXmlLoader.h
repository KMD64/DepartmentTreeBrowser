#ifndef DEPARTMENTITEMXMLLOADER_H
#define DEPARTMENTITEMXMLLOADER_H

#include "DepartmentItem.h"

#include <qdom.h>



class DepartmentItemXmlLoader
{
    QPointer<DepartmentItem> _item;
public:
    DepartmentItemXmlLoader(DepartmentItem *item);

    bool load(const QDomNode &node,QString *errmsg=nullptr);
    QDomNode save(QDomDocument &doc) const;
};

#endif // DEPARTMENTITEMXMLLOADER_H
