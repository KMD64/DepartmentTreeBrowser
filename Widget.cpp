#include "Widget.h"
#include "ui_Widget.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QResizeEvent>
using namespace std::chrono_literals;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    mdl(new TreeModel(this))
{
    ui->setupUi(this);
    ui->treeView->setModel(mdl);
    ui->undoView->setStack(mdl->undoStack());
}

Widget::~Widget()
{
    delete ui;
    mdl->deleteLater();
}

void Widget::on_browseButton_clicked(bool)
{
    QString filePath = QDir::homePath();

    if(QString text=ui->FilePathEdit->text();!text.isEmpty()){
        filePath=text;
    }
    filePath = QFileDialog::getOpenFileName(this,"Open File",filePath,"Xml File (*.xml)");
    if(!filePath.isEmpty()){
        ui->FilePathEdit->setText(filePath);
    }
}

void Widget::on_loadButton_clicked(bool)
{

//    if(!mdl){
//        mdl = new TreeModel(this);
//        ui->treeView->setModel(mdl);
//    }

}

void Widget::on_addDepartmentButton_clicked(bool)
{
    mdl->insertRow(mdl->rowCount(QModelIndex()));
}

void Widget::on_addEmployeeButton_clicked(bool)
{
    auto sModel = ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto parent = sModel->selectedRows()[0];
        mdl->insertRow(mdl->rowCount(parent),parent);
    }
}

void Widget::on_deleteButton_clicked(bool)
{
    auto sModel = ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto items = sModel->selectedRows();
        mdl->removeRows(items[0].row(),items.size(),items[0].parent());
    }
}

void Widget::on_undoButton_clicked(bool)
{
    mdl->undoStack()->undo();
}

void Widget::on_redoButton_clicked(bool)
{
    mdl->undoStack()->redo();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    ui->verticalLayoutWidget->resize(event->size());
}
