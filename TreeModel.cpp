#include "TreeModel.h"

#include <QMetaProperty>
#include <cmath>
#include <QDebug>
#include <QLinkedList>
//-------------------------------------------------------------
//Helper functions for getting data from index
//-------------------------------------------------------------
TreeItem *TreeModel::itemFromIndex(const QModelIndex &index) const{
    if(index.isValid()){
        return static_cast<TreeItem*>(index.internalPointer());
    }
    return _root;
}

TreeItemFactory::ItemType TreeModel::typeFromIndex(const QModelIndex &parent) const{
    if(parent.isValid()){
        return TreeItemFactory::Employee;
    }
    else{
        return TreeItemFactory::Department;
    }
}
//-------------------------------------------------------------
//TreeModel realization
//-------------------------------------------------------------
TreeModel::TreeModel(QObject *parent):
    QAbstractItemModel (parent),
    _root(new TreeItem(this))
{

    int employeeCount[]{4,3,5};
    for(int i=0;i<3;++i){
        auto item = TreeItemFactory::createItem(TreeItemFactory::Department,_root);
        item->setProperty("name",QString("Department #%1").arg(i));
        for(int j=0;j<employeeCount[i];++j){
            TreeItem* employee = TreeItemFactory::createItem(TreeItemFactory::Employee,item);
            employee->setProperty("salary",j*100000+i*100);
            item->addChild(employee);
        }
        _root->addChild(item);
    }

    stk = new QUndoStack(this);
}

TreeModel::~TreeModel()
{
    delete _root;
}


QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row,column,parent)){
        return QModelIndex();
    }
    auto item = itemFromIndex(parent);
    return createIndex(row,column,item->child_at(row));

}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    auto item = static_cast<TreeItem*>(child.internalPointer());
    auto parent = item->parentItem();
    if(parent == _root)
        return QModelIndex();
    else{
        int row = parent->parentItem()->childItems().indexOf(parent);
        if(row < 0)
            return QModelIndex();
        return createIndex(row,0,parent);
    }
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return _root->childItems().size();
    auto item = static_cast<TreeItem*>(parent.internalPointer());
    return item->childItems().size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto item = static_cast<TreeItem*>(index.internalPointer());
    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item->metaObject()->property(index.column()+1).read(item);
    }
    return QVariant();
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()){
        return Qt::NoItemFlags;
    }
    Qt::ItemFlags flags;
    auto item = static_cast<QObject*>(index.internalPointer());
    auto property = item->metaObject()->property(index.column()+1);
    if(property.isValid()){
        flags|=Qt::ItemIsEnabled|Qt::ItemIsSelectable;
        if(property.isWritable())
            flags|=Qt::ItemIsEditable;
    }
    return flags;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if(role == Qt::EditRole){
    auto item = static_cast<QObject*>(index.internalPointer());
    auto property = item->metaObject()->property(index.column()+1);
    if(!property.isValid())
        return false;
    if(!property.isWritable()){
        return false;
    }
    if(property.isReadable()){
        if(auto propValue = property.read(item);propValue == value){
            return false;
        }
    }
    property.write(item,value);
    return true;
    }
    return false;
}

bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{

    TreeItem * parentItem = itemFromIndex(parent);
    int startRow = std::clamp(row,0,parentItem->children().size());
    stk->push(new RowsInsertionCommand(startRow,count,parent,this));
    return true;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TreeItem* item = itemFromIndex(parent);
    int realCount = row+count-item->childItems().size();
    stk->push(new RowsRemoveCommand(row,std::max(count,realCount),parent,this));
    return true;
}

QUndoStack *TreeModel::undoStack()
{
    return stk;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//Commands realization
//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//Rows Insertion command
//--------------------------------------------------------------------------------------------------------------------------------------------
TreeModel::RowsInsertionCommand::RowsInsertionCommand(int row, int count, const QModelIndex &parentIndex, TreeModel *model,QUndoCommand* parent):
    QUndoCommand (parent),
    _row(row),
    _count(count),
    _parentIndex(parentIndex),
    _mdl(model)
{
    setText(tr("Insertion: %1").arg(TreeItemFactory::getItemTypeString(_mdl->typeFromIndex(parentIndex))));
}

void TreeModel::RowsInsertionCommand::undo()
{
    //restoring _parentIndex
    for(_parentIndex = QModelIndex();!_rootOffset.isEmpty();_rootOffset.pop()){
        _parentIndex = _mdl->index(_rootOffset.top(),0,_parentIndex);
    }
    TreeItem *parentItem = _mdl->itemFromIndex(_parentIndex);
    _mdl->beginRemoveRows(_parentIndex,_row,_row+_count-1);
    parentItem->removeChild(_row,_count);
    _mdl->endRemoveRows();
}

void TreeModel::RowsInsertionCommand::redo()
{
    //save parent position
    for(QModelIndex index = _parentIndex;index.isValid();index = index.parent()){
        _rootOffset.push(index.row());
    }
    TreeItem *parentItem = _mdl->itemFromIndex(_parentIndex);
    if(!parentItem->mapped())return;
    _mdl->beginInsertRows(_parentIndex,_row,_row+_count-1);
    for(int i=0;i<_count;++i){
        parentItem->insertChild(_row+i,TreeItemFactory::createItem(_mdl->typeFromIndex(_parentIndex)));
    }
    _mdl->endInsertRows();
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//Rows Removing command
//--------------------------------------------------------------------------------------------------------------------------------------------
TreeModel::RowsRemoveCommand::RowsRemoveCommand(int row, int count, const QModelIndex &parentIndex, TreeModel *model,QUndoCommand* parent):
    QUndoCommand (parent),
    _row(row),
    _count(count),
    _parentIndex(parentIndex),
    _mdl(model)
{
    setText(tr("Removing: %1 to %2").arg(row).arg(row+count-1));
}

void TreeModel::RowsRemoveCommand::undo()
{
    //restoring _parentIndex
    for(_parentIndex = QModelIndex();!_rootOffset.isEmpty();_rootOffset.pop()){
        _parentIndex = _mdl->index(_rootOffset.top(),0,_parentIndex);
    }
    _mdl->beginInsertRows(_parentIndex,_row,_row+_count-1);
    //Restoring deleted rows
    auto parentItem = _mdl->itemFromIndex(_parentIndex);
    auto type = _mdl->typeFromIndex(_parentIndex);
    for(int rowOffset=0;rowOffset<_count;++rowOffset){
        auto item = TreeItemFactory::createItem(type);
        //Filling writable properies
        int propIndex=0;
        QMetaProperty property;
        for(auto data:items[rowOffset]){
            //find next suitable property, index points at next property
            do{
                property = item->metaObject()->property(propIndex++);
            }while(!property.isReadable()||!property.isWritable());
            property.write(item,data);
        }
        parentItem->insertChild(_row+rowOffset,item);
    }
    _mdl->endInsertRows();
}

void TreeModel::RowsRemoveCommand::redo()
{
    //First recursively delete all rows from index
    for (int offset=0;offset<_count;++offset) {
        auto index = _mdl->index(_row+offset,0,_parentIndex);//mark the target
        auto count = _mdl->rowCount(index);
        if(count==0)continue;//No need to delete
        //Here comes the recursion
        _mdl->stk->push(new RowsRemoveCommand(0,count,index,_mdl,this));
    }
    //need to save relative position of parent
    for(QModelIndex index = _parentIndex;index.isValid();index=index.parent()){
        _rootOffset.push(index.row());
    }
    //start removing rows
    _mdl->beginRemoveRows(_parentIndex,_row,_row+_count-1);
    auto parentItem = _mdl->itemFromIndex(_parentIndex);
    //Copying writable properties from items at range [row, row+count-1]
    for(int i=0;i<_count;++i){
       auto childItem = parentItem->child_at(_row+i);
       QLinkedList<QVariant> row;
       for(int j = 0;j<childItem->metaObject()->propertyCount();++j){
           auto itemProperty = childItem->metaObject()->property(j);
           if(itemProperty.isWritable()&&itemProperty.isReadable()){
               row.append(itemProperty.read(childItem));
           }
       }
       items.append(row);
    }
    //Removing rows from parent
    parentItem->removeChild(_row,_count);
    _mdl->endRemoveRows();

}
