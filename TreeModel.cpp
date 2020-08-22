#include "TreeModel.h"

#include <QMetaProperty>
#include <cmath>
#include <QLinkedList>
//-------------------------------------------------------------
//Helper functions
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

void TreeModel::emitDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight, QVector<int> roles)
{
    emit dataChanged(topLeft,bottomRight,roles);
}


//-------------------------------------------------------------
//TreeModel realization
//-------------------------------------------------------------
TreeModel::TreeModel(QObject *parent):
    QAbstractItemModel (parent),
    _stk(new QUndoStack(this)),
    _root(new TreeItem(this))
{

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
    //internal pointer = pointer to parent
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
    _stk->push(new SetDataCommand(index,role,value,this));
    return true;
}

bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{

    TreeItem * parentItem = itemFromIndex(parent);
    int startRow = std::clamp(row,0,parentItem->children().size());
    _stk->push(new RowsInsertionCommand(startRow,count,parent,this));
    return true;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TreeItem* item = itemFromIndex(parent);
    int realCount = row+count-item->childItems().size();
    _stk->push(new RowsRemoveCommand(row,std::max(count,realCount),parent,this));
    return true;
}

QUndoStack *TreeModel::undoStack()
{
    return _stk;
}

const TreeItem *TreeModel::root()
{
    return _root;
}

void TreeModel::setRoot(TreeItem *root)
{
    beginResetModel();
    _root->deleteLater();
    _root = root;
    undoStack()->clear();
    endResetModel();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//Commands realization
//--------------------------------------------------------------------------------------------------------------------------------------------
TreeModel::ModelCommand::ModelCommand(const QModelIndex &index, TreeModel *mdl, QUndoCommand *parent):
    QUndoCommand (parent),
    _mdl(mdl)
{
    //saving relative item position
    for(auto cindex = index;cindex.isValid();cindex = cindex.parent()){
        _rootOffset.append(cindex.row());
    }
}

QString TreeModel::ModelCommand::offsetString()
{
    QStringList strs;
    strs.append("root");
    for(int i:_rootOffset){
        strs.append(QString::number(i));
    }
    return strs.join(",");
}

QModelIndex TreeModel::ModelCommand::getIndex()
{
    auto index = QModelIndex();
    for(auto iter = _rootOffset.rbegin();iter!=_rootOffset.rend();++iter)
        index = _mdl->index(*iter,0,index);
    return index;
}


//--------------------------------------------------------------------------------------------------------------------------------------------
//Rows Insertion command
//--------------------------------------------------------------------------------------------------------------------------------------------
TreeModel::RowsInsertionCommand::RowsInsertionCommand(int row, int count, const QModelIndex &parentIndex, TreeModel *model,QUndoCommand* parent):
    ModelCommand(parentIndex,model,parent),
    _row(row),
    _count(count)
{
    setText(tr("Insertion: %1: (%2,%3)").arg(offsetString()).arg(row).arg(row+count-1));
}

void TreeModel::RowsInsertionCommand::undo()
{
    //restoring parentIndex
    auto parentIndex = getIndex();
    auto parentItem = _mdl->itemFromIndex(parentIndex);
    _mdl->beginRemoveRows(parentIndex,_row,_row+_count-1);
    parentItem->removeChild(_row,_count);
    _mdl->endRemoveRows();
}

void TreeModel::RowsInsertionCommand::redo()
{
    auto parentIndex = getIndex();
    TreeItem* parentItem = _mdl->itemFromIndex(parentIndex);
    if(!parentItem->mapped()){
        //does nothing
        this->setObsolete(true);
        return;
    }
    _mdl->beginInsertRows(parentIndex,_row,_row+_count-1);
    for(int i=0;i<_count;++i){
        parentItem->insertChild(_row+i,TreeItemFactory::createItem(_mdl->typeFromIndex(parentIndex)));
    }
    _mdl->endInsertRows();
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//Rows Removing command
//--------------------------------------------------------------------------------------------------------------------------------------------
TreeModel::RowsRemoveCommand::RowsRemoveCommand(int row, int count, const QModelIndex &parentIndex, TreeModel *model,QUndoCommand* parent):
    ModelCommand (parentIndex,model,parent),
    _row(row),
    _count(count)
{
    setText(tr("Removing: %1: (%2,%3)").arg(offsetString()).arg(row).arg(row+count-1));
}

void TreeModel::RowsRemoveCommand::undo()
{
    //restoring _parentIndex

    auto parentIndex = getIndex();
    auto parentItem = _mdl->itemFromIndex(parentIndex);
    _mdl->beginInsertRows(parentIndex,_row,_row+_count-1);
    //Restoring deleted rows
    auto type = _mdl->typeFromIndex(parentIndex);
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

    auto parentIndex = getIndex();
    auto parentItem = _mdl->itemFromIndex(parentIndex);
    //First recursively delete all rows from index
    for (int offset=0;offset<_count;++offset) {
        auto index = _mdl->index(_row+offset,0,parentIndex);//mark the target
        auto count = _mdl->rowCount(index);
        if(count==0)continue;//No need to delete
        //Here comes the recursion
        _mdl->_stk->push(new RowsRemoveCommand(0,count,index,_mdl,this));
    }

    //start removing rows
    _mdl->beginRemoveRows(parentIndex,_row,_row+_count-1);
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


TreeModel::SetDataCommand::SetDataCommand(const QModelIndex &index, int role,const QVariant &value, TreeModel *mdl, QUndoCommand *command):
    ModelCommand(index,mdl,command),
    _column(index.column()),
    _role(role),
    _value(value)
{
    _oldValue = _mdl->data(index,role);

    setText(tr("Data change:%1:%3:%4->%5").arg(offsetString()).arg(_column).arg(_oldValue.toString()).arg(_value.toString()));
}

void TreeModel::SetDataCommand::undo()
{
    auto index = getIndex();
    auto item = _mdl->itemFromIndex(index);
    switch(_role){
    case Qt::DisplayRole:
    case Qt::EditRole:{
        item->metaObject()->property(_column+1).write(item,_oldValue);
        _mdl->emitDataChange(index,index);
        }
    }
}

void TreeModel::SetDataCommand::redo()
{
    auto index = getIndex();
    auto item = _mdl->itemFromIndex(index);
    switch(_role){
    case Qt::DisplayRole:
    case Qt::EditRole:{
        item->metaObject()->property(_column+1).write(item,_value);
        _mdl->emitDataChange(index,index);
    }
    }
}


