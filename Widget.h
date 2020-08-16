#ifndef WIDGET_H
#define WIDGET_H

#include "TreeModel.h"

#include <QThread>
#include <QUndoStack>
#include <QWidget>
#include <qtreeview.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget() override;

private:
    Ui::Widget *ui;
    TreeModel *mdl;
    QTreeView *_view;
private slots:
    //button slots
    void on_browseButton_clicked(bool);
    void on_loadButton_clicked(bool);
    void on_addDepartmentButton_clicked(bool);
    void on_addEmployeeButton_clicked(bool);
    void on_deleteButton_clicked(bool);
    void on_undoButton_clicked(bool);
    void on_redoButton_clicked(bool);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // WIDGET_H
