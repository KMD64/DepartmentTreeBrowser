#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>

class TreeItem : public QObject
{
    Q_OBJECT
public:
    explicit TreeItem(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TREEITEM_H
