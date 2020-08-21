#include "DepartmentItem.h"
#include "DepartmentItemXmlLoader.h"
#include "RootItemXmlLoader.h"



RootItemXmlLoader::RootItemXmlLoader(TreeItem *root):_root(root)
{

}

QDomNode RootItemXmlLoader::save(QDomDocument &doc) const
{
    auto node = doc.createElement("departments");
    for(int i=0;i<_root->childItems().count();++i){
        auto dep = dynamic_cast<DepartmentItem *>(_root->child_at(i));
        node.appendChild(DepartmentItemXmlLoader(dep).save(doc));
    }
    return node;
}

bool RootItemXmlLoader::load(const QDomNode &node, QString *errmsg)
{
    //root node is element node with name "departments"
    try{
        if(!node.isElement())throw QObject::tr("Invalid node type");
        if(node.nodeName().toLower()!="departments")throw QObject::tr("Invalid node name: %1").arg(node.nodeName());

        //all child nodes are department nodes
        auto childNodes = node.childNodes();
        for(int i=0;i<childNodes.size();++i){
            auto depNode = childNodes.at(i);
            DepartmentItem* dep = new DepartmentItem(_root);
            if(!DepartmentItemXmlLoader(dep).load(depNode,errmsg)){
                dep->deleteLater();
                throw errmsg;
            }
            _root->addChild(dep);
        }
    }catch(const QString &str){
        *errmsg = str;
        return false;
    }
    return true;
}
