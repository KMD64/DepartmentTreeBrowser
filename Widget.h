#ifndef WIDGET_H
#define WIDGET_H

#include "TreeModel.h"

#include <QMainWindow>
#include <QThread>
#include <QUndoStack>
#include <QWidget>
#include <qtreeview.h>

namespace Ui {
class MainWindow;
}

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget() override;

private:
    QString filePath;
    Ui::MainWindow *_ui;
    TreeModel *_mdl;
    QTreeView *_view;
    bool askPath(bool forSave = false);
private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionExit_triggered();
    void on_actionAddDepartment_triggered();
    void on_actionAddEmployee_triggered();
    void on_actionDelete_triggered();


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // WIDGET_H
