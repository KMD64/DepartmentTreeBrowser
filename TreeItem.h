#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QPointer>
#include <QVector>
class TreeItem:public QObject
{
    Q_OBJECT
protected:
    QPointer<TreeItem> _parentItem;
    QVector<QPointer<TreeItem>> _childItems;
public:
    explicit TreeItem(QObject *parent = nullptr);
    virtual ~TreeItem();

    TreeItem* parentItem() const;
    void setParentItem(TreeItem *parentItem);

    bool addChild(TreeItem* childItem);
    bool removeChild(int i,int n=1);
    bool insertChild(int i,TreeItem* item);
    TreeItem* child_at(int i);

    virtual bool mapped();
    const QVector<QPointer<TreeItem>> &childItems() const;

};

#endif // TREEITEM_H
