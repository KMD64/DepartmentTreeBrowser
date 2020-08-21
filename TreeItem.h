#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QPointer>
#include <QVector>
class TreeItem:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    QPointer<TreeItem> _parentItem;
    QString _name;
    QVector<TreeItem *> _childItems;
protected:
    void setParentItem(TreeItem *parentItem);
public:
    explicit TreeItem(QObject *parent = nullptr);
    virtual ~TreeItem();

    TreeItem* parentItem() const;

    bool addChild(TreeItem* childItem);
    bool removeChild(int i,int n=1);
    bool insertChild(int i,TreeItem* item);
    TreeItem* child_at(int i);

    virtual bool mapped();
    QVector<TreeItem *> &childItems();

    QString name() const;
    void setName(const QString &name);


};

#endif // TREEITEM_H
