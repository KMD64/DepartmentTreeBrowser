#include "RootItemXmlLoader.h"
#include "Widget.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QResizeEvent>
#include <QDomDocument>
#include <QTextStream>

Widget::Widget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mdl(new TreeModel(this))
{
    ui->setupUi(this);
    _view = ui->treeView;
    _view->setModel(mdl);
    _view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //provide undo/redo actions
    auto undoAction = mdl->undoStack()->createUndoAction(ui->menuEdit,"Undo:");
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    auto redoAction = mdl->undoStack()->createRedoAction(ui->menuEdit,"Redo:");
    redoAction->setShortcut(QKeySequence("Ctrl+Y"));
    ui->menuEdit->insertAction(ui->menuEdit->actions().at(0),undoAction);
    ui->menuEdit->insertAction(undoAction,redoAction);
    ui->undoView->setStack(mdl->undoStack());
}

Widget::~Widget()
{
    delete ui;
    delete mdl;
}

bool Widget::askPath(bool forSave)
{
    if(filePath.isEmpty()){
        filePath=QDir::homePath();
    }
    QString newFilePath;
    if(forSave){
        newFilePath = QFileDialog::getSaveFileName(this,tr("Save file"),filePath,"Xml File (*.xml)");
    }
    else{
        newFilePath = QFileDialog::getOpenFileName(this,tr("Open File"),filePath,"Xml File (*.xml)");
    }

    if(!newFilePath.isEmpty()){
        filePath = newFilePath;
        return true;
    }
    else{
        return false;
    }
}
//Action commands
void Widget::on_actionNew_triggered()
{
    filePath.clear();
    mdl->setRoot(new TreeItem(mdl));

}

void Widget::on_actionOpen_triggered()
{
    if(!askPath())return;
    //open file
    QFile file(filePath);
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

        auto rootNode = doc.documentElement();
        if(!RootItemXmlLoader(root).load(rootNode,&msg)){
            throw msg;
        }
    }catch(const QString &str){
        root->deleteLater();
        QMessageBox::warning(this,tr("XML processing error"),str);
        return;
    }catch(std::exception e){
        root->deleteLater();
        QMessageBox::warning(this,tr("Exception"),e.what());
        return;
    }

    mdl->setRoot(root);
}

void Widget::on_actionSave_triggered()
{
    if(filePath.isEmpty()&&!askPath(true))return;
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,tr("File open error"),tr("Failed to open the file"));
        return;
    }
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml","version = \"1.0\" encoding = \"UTF-8\""));
    doc.appendChild(RootItemXmlLoader(const_cast<TreeItem *>(mdl->root())).save(doc));
    QTextStream stream(&file);
    doc.save(stream,4);
}

void Widget::on_actionSaveAs_triggered()
{
    auto path = filePath;
    if(!askPath(true))return;
    on_actionSave_triggered();
    if(!path.isEmpty())
        filePath = path;
}

void Widget::on_actionExit_triggered()
{
    this->close();
}

void Widget::on_actionAddDepartment_triggered()
{
    mdl->insertRow(mdl->rowCount(QModelIndex()));
}

void Widget::on_actionAddEmployee_triggered()
{
    auto sModel = ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto parent = sModel->selectedRows()[0];
        mdl->insertRow(mdl->rowCount(parent),parent);
    }
}

void Widget::on_actionDelete_triggered()
{
    auto sModel = ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto items = sModel->selectedRows();
        mdl->removeRows(items[0].row(),items.size(),items[0].parent());
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    ui->horizontalLayoutWidget->resize(event->size());
}
