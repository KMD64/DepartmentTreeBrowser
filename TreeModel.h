#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "TreeItemFactory.h"
#include <QAbstractItemModel>
#include <QStack>
#include <QUndoStack>
//-------------------------------------------------------------------------------
//TreeModel
//-------------------------------------------------------------------------------
class TreeModel:public QAbstractItemModel
{
    Q_OBJECT
    QUndoStack* stk;
    TreeItem *_root;
protected:
//-------------------------------------------------------------------------------
//QUndoCommand implementations
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    class RowsInsertionCommand:public QUndoCommand{
        int _row;
        int _count;
        QModelIndex _parentIndex;
        TreeModel *_mdl;
        QStack<int> _rootOffset;
    public:
        RowsInsertionCommand(int row,int count,const QModelIndex &parentIndex,TreeModel *model,QUndoCommand *parent=nullptr);
        // QUndoCommand interface
    public:
        void undo() override;
        void redo() override;
    };
//-------------------------------------------------------------------------------
    class RowsRemoveCommand:public QUndoCommand{
        int _row;
        int _count;
        QModelIndex _parentIndex;
        TreeModel *_mdl;
        QList<QLinkedList<QVariant>> items;
        QStack<int> _rootOffset;
    public:
        RowsRemoveCommand(int row,int count,const QModelIndex& parentIndex,TreeModel *model,QUndoCommand *parent=nullptr);
        // QUndoCommand interface
    public:
        void undo() override;
        void redo() override;
    };
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
public:
    TreeModel(QObject* parent = nullptr);
    ~TreeModel() override;
//-------------------------------------------------------------------------------
// QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
//-------------------------------------------------------------------------------
public:
    QUndoStack *undoStack();
    const TreeItem *root();
protected:
    TreeItem* itemFromIndex(const QModelIndex& parent) const;
    TreeItemFactory::ItemType typeFromIndex(const QModelIndex& parent) const;
};

#endif // TREEMODEL_H
