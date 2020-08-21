#ifndef ROOTITEMXMLLOADER_H
#define ROOTITEMXMLLOADER_H

#include "TreeItem.h"

#include <QPointer>
#include <qdom.h>



class RootItemXmlLoader
{
    QPointer<TreeItem> _root;
public:
    RootItemXmlLoader(TreeItem* root);
    RootItemXmlLoader(const TreeItem * root);
    QDomNode save(QDomDocument &doc) const ;
    bool load(const QDomNode &node,QString *errmsg=nullptr);
};

#endif // ROOTITEMXMLLOADER_H
