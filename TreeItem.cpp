#include "TreeItem.h"

const QVector<QPointer<TreeItem> > &TreeItem::childItems() const
{
    return _childItems;
}

QString TreeItem::name() const
{
    return _name;
}

void TreeItem::setName(const QString &name)
{
    _name = name;
}

TreeItem::TreeItem(QObject *parent):QObject(parent)
{

}

TreeItem::~TreeItem()
{
    qDeleteAll(_childItems);
}
void TreeItem::setParentItem(TreeItem *parentItem)
{
    _parentItem = parentItem;
}

bool TreeItem::addChild(TreeItem *childItem)
{
    if(mapped()){
        childItem->setParentItem(this);
        _childItems.append(childItem);
    }
    return mapped();
}

bool TreeItem::removeChild(int r, int n)
{
    if(mapped()){
        for(int i=0;i<n;++i){
            _childItems[r+i]->deleteLater();
        }
        _childItems.remove(r,n);
    }
    return mapped();
}

bool TreeItem::insertChild(int r,TreeItem* item)
{
    if(mapped()){
        item->setParentItem(this);
        _childItems.insert(r,item);
    }
    return mapped();
}

TreeItem *TreeItem::child_at(int i)
{
    return _childItems.at(i);
}

bool TreeItem::mapped()
{
    return true;
}



TreeItem *TreeItem::parentItem() const
{
    return _parentItem;
}
