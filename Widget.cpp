#include "DepartmentItem.h"
#include "DepartmentItemXmlLoader.h"
#include "Widget.h"
#include "ui_Widget.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QResizeEvent>
#include <QDomDocument>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    mdl(new TreeModel(this))
{
    ui->setupUi(this);
    _view = ui->treeView;
    _view->setModel(mdl);
    ui->undoView->setStack(mdl->undoStack());
    _view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

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
    //open file
    QFile file(ui->FilePathEdit->text());
    if(!file.open(QFile::ReadOnly|QFile::Text)){
        //Message
        QMessageBox::warning(this,tr("File open error"),tr("Failed to open the file"));
        return;
    }
    //setting up document
    TreeItem *root = new TreeItem();
    try{
        QDomDocument doc;
        QString msg;
        int row,col;
        if(!doc.setContent(&file,false,&msg,&row,&col))throw tr("Error on row %1, column %2:\n %3").arg(row).arg(col).arg(msg);
        //preparing root

        root->setName("root");
        auto rootNode = doc.documentElement();
        if(rootNode.nodeName().toLower()!="departments"){
            throw tr("Invalid node name: %1").arg(rootNode.nodeName());
        }
        auto departmentNodes = rootNode.childNodes();
        for(int i=0;i<departmentNodes.size();++i){
            auto depNode = departmentNodes.at(i);
            DepartmentItem* item = new DepartmentItem(root);
            if(!DepartmentItemXmlLoader(item).load(depNode,&msg))throw msg;
            root->addChild(item);
        }
    }catch(const QString &str){
        root->deleteLater();
        QMessageBox::warning(this,tr("XML processing error"),str);
    }
    mdl->setRoot(root);
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
