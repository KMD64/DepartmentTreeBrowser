#ifndef TREEITEMFACTORY_H
#define TREEITEMFACTORY_H

#include "TreeItem.h"



class TreeItemFactory
{
private:
    TreeItemFactory();
public:    
    enum ItemType{Default=0,Department,Employee};
    static TreeItem *createItem(ItemType type, TreeItem *parent = nullptr);
    static QString getItemTypeString(ItemType type);
};

#endif // TREEITEMFACTORY_H
