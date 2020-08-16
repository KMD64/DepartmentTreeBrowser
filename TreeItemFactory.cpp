#include "DepartmentItem.h"
#include "TreeItemFactory.h"
#include <QDebug>
#include <QVariant>
TreeItem *TreeItemFactory::createItem(ItemType type,TreeItem *parent,const QMap<QString, QString> &args)
{
    TreeItem* item;
    switch(type){
    case Department:{
        item = new DepartmentItem(parent);

        break;
    }
    case Employee:{
        item = new EmploymentItem(parent);break;
    }
    default:{item = new TreeItem(parent);}
    }
    //fill object
    for(auto &key:args){
        auto value = args.value(key);
        bool result = item->setProperty(key.toStdString().data(),args.value(key));
        qDebug()<<"Insertion data:"<<key<<" "<<value<<":"<<result;
    }
    item->setParentItem(parent);
    return item;
}

QString TreeItemFactory::getItemTypeString(TreeItemFactory::ItemType type)
{
    switch(type){
        case Department:return "DepartmentItem";
        case Employee:return "EmploymentItem";
    default: return "TreeItem";
    }
}
