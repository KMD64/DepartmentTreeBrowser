#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "TreeItemFactory.h"
#include <QAbstractItemModel>
#include <QLinkedList>
#include <QStack>
#include <QUndoStack>
//===============================================================================
//TreeModel
//===============================================================================
class TreeModel:public QAbstractItemModel
{
    Q_OBJECT
    QUndoStack* _stk;
    TreeItem *_root;
protected:
//===============================================================================
//QUndoCommand implementations
//===============================================================================

class ModelCommand:public QUndoCommand{
protected:
    ModelCommand(const QModelIndex &index,TreeModel *mdl,QUndoCommand *parent = nullptr);
    QLinkedList<int> _rootOffset;
    TreeModel *_mdl;
    QString offsetString();
    QModelIndex getIndex();

};
//-------------------------------------------------------------------------------
    class RowsInsertionCommand:public ModelCommand{
        int _row;
        int _count;
    public:
        RowsInsertionCommand(int row,int count,const QModelIndex &parentIndex,TreeModel *model,QUndoCommand *parent=nullptr);
        // QUndoCommand interface
    public:
        void undo() override;
        void redo() override;
    };
//-------------------------------------------------------------------------------
    class RowsRemoveCommand:public ModelCommand{
        int _row;
        int _count;
        QList<QLinkedList<QVariant>> items;

    public:
        RowsRemoveCommand(int row,int count,const QModelIndex& parentIndex,TreeModel *model,QUndoCommand *parent=nullptr);
        // QUndoCommand interface
    public:
        void undo() override;
        void redo() override;
    };
//-------------------------------------------------------------------------------
class SetDataCommand:public ModelCommand{
    int _column;
    int _role;
    QVariant _value;
    QVariant _oldValue;
public:
    SetDataCommand(const QModelIndex &index, int role, const QVariant &value, TreeModel* mdl, QUndoCommand *command=nullptr);

    // QUndoCommand interface
public:
    void undo() override;
    void redo() override;
};
//===============================================================================
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
    void setRoot(TreeItem* root);
protected:
    TreeItem* itemFromIndex(const QModelIndex& parent) const;
    TreeItemFactory::ItemType typeFromIndex(const QModelIndex& parent) const;
    void emitDataChange(const QModelIndex& topLeft, const QModelIndex &bottomRight,QVector<int> roles = QVector<int>());
};

#endif // TREEMODEL_H
