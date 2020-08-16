#include "EmploymentItemXmlLoader.h"
#include <QDebug>
EmploymentItemXmlLoader::EmploymentItemXmlLoader(EmploymentItem* item):_item(item)
{

}

EmploymentItemXmlLoader::~EmploymentItemXmlLoader()
{
}

bool EmploymentItemXmlLoader::extractText(const QDomNode &node, QString &string)
{
    auto textNode = node.firstChild();
    if(textNode.isText()){
        string = textNode.nodeValue();
        return true;
    }
    throw QObject::tr("Invalid node type: %1").arg(textNode.nodeType());
}

bool EmploymentItemXmlLoader::load(const QDomNode &node,QString *const errmsg)
{
    try{
        if(!node.isElement())throw QObject::tr("Not an element");
        if(node.nodeName().toLower()!="employment")throw QObject::tr("Wrong element name: %1").arg(node.nodeName());
        auto childNodes = node.childNodes();
        //preparing data
        QStringList name{"","",""};
        QString salary;
        QString function;
        for(int i=0;i<childNodes.size();++i){
            auto child = childNodes.at(i);
            auto childName = child.nodeName().toLower();
            if(childName=="surname"){
                extractText(child,name[0]);
                continue;
            }
            if(childName=="name"){
                extractText(child,name[1]);
                continue;
            }
            if(childName=="middlename"){
                extractText(child,name[2]);
                continue;
            }
            if(childName=="salary"){
                extractText(child,salary);
                continue;
            }
            if(childName=="function"){
                extractText(child,function);
                continue;
            }
        }
        _item->setName(name.join(" ").trimmed());
        _item->setFunction(function);
        _item->setSalary(salary.toInt());
        return true;
    }catch(const QString &str){
        if(errmsg)
            *errmsg = str;
        return false;
    }
}

QDomNode EmploymentItemXmlLoader::save()
{
    //Not implemented yet
    return QDomNode();
}

