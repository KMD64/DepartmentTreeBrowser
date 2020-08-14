#include "DepartmentItem.h"
#include "TreeItemFactory.h"


TreeItem *TreeItemFactory::createItem(ItemType type, TreeItem *parent)
{
    TreeItem* item;
    switch(type){
    case Department:{item = new DepartmentItem(parent);break;}
    case Employee:{item = new EmploymentItem(parent);break;}
    default:{item = new TreeItem(parent);}
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
