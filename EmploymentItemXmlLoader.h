#ifndef EMPLOYMENTITEMXMLLOADER_H
#define EMPLOYMENTITEMXMLLOADER_H

#include "EmploymentItem.h"

#include <qdom.h>



class EmploymentItemXmlLoader
{
    QPointer<EmploymentItem> _item;
public:
    EmploymentItemXmlLoader(EmploymentItem *item);
    ~EmploymentItemXmlLoader();
    bool extractText(const QDomNode &node,QString& string);
    bool load(const QDomNode &node, QString *const errmsg=nullptr);
    QDomNode save(QDomDocument &doc) const;
};

#endif // EMPLOYMENTITEMXMLLOADER_H
