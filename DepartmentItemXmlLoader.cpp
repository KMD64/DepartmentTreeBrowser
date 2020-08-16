#include "DepartmentItemXmlLoader.h"
#include "EmploymentItemXmlLoader.h"

DepartmentItemXmlLoader::DepartmentItemXmlLoader(DepartmentItem *item):_item(item)
{

}

bool DepartmentItemXmlLoader::load(const QDomNode &node, QString *errmsg)
{
    try{
        if(!node.isElement())throw QObject::tr("Invalid node type");
        if(node.nodeName().toLower()!="department")throw QObject::tr("Invalid node name: %1").arg(node.nodeName());
        auto nameNode = node.attributes().namedItem("name");
        if(nameNode == QDomNode())throw QObject::tr("Node attribute called \"name\" not present");
        _item->setName(nameNode.nodeValue());
        //now we need employments child node
        auto employeesNode = node.namedItem("employments");
        if(employeesNode == QDomNode())throw QObject::tr("Employments node not present");
        //constructing employment items from childs
        auto employees = employeesNode.childNodes();
        for(int i=0;i<employees.size();++i){
            auto employeeNode = employees.at(i);
            QPointer<EmploymentItem> item = new EmploymentItem(_item);
            if(!EmploymentItemXmlLoader(item).load(employeeNode,errmsg)){
                item->deleteLater();
                throw errmsg;
            }
            _item->addChild(item);
        }
    }catch(const QString &str){
        *errmsg = str;
        return false;
    }
    return true;
}

