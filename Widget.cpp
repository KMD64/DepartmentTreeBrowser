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
    _ui(new Ui::MainWindow),
    _mdl(new TreeModel(this))
{
    _ui->setupUi(this);
    _view = _ui->treeView;
    _view->setModel(_mdl);
    _view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //provide undo/redo actions
    auto undoAction = _mdl->undoStack()->createUndoAction(_ui->menuEdit,"Undo:");
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    auto redoAction = _mdl->undoStack()->createRedoAction(_ui->menuEdit,"Redo:");
    redoAction->setShortcut(QKeySequence("Ctrl+Y"));
    _ui->menuEdit->insertAction(_ui->menuEdit->actions().at(0),undoAction);
    _ui->menuEdit->insertAction(undoAction,redoAction);
    _ui->undoView->setStack(_mdl->undoStack());
}

Widget::~Widget()
{
    delete _ui;
    delete _mdl;
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
    _mdl->setRoot(new TreeItem(_mdl));

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

    _mdl->setRoot(root);
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
    doc.appendChild(RootItemXmlLoader(const_cast<TreeItem *>(_mdl->root())).save(doc));
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
    _mdl->insertRow(_mdl->rowCount(QModelIndex()));
}

void Widget::on_actionAddEmployee_triggered()
{
    auto sModel = _ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto parent = sModel->selectedRows()[0];
        _mdl->insertRow(_mdl->rowCount(parent),parent);
    }
}

void Widget::on_actionDelete_triggered()
{
    auto sModel = _ui->treeView->selectionModel();
    if(sModel->hasSelection()){
        auto items = sModel->selectedRows();
        _mdl->removeRows(items[0].row(),items.size(),items[0].parent());
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    _ui->horizontalLayoutWidget->resize(_ui->centralwidget->size());
}
