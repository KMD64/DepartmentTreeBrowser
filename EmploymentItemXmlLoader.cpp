#include "EmploymentItemXmlLoader.h"
#include <QDebug>
#include <QMetaProperty>
EmploymentItemXmlLoader::EmploymentItemXmlLoader(EmploymentItem* item):_item(item)
{

}

EmploymentItemXmlLoader::~EmploymentItemXmlLoader()
{
}

bool EmploymentItemXmlLoader::extractText(const QDomNode &node, QString &string)
{
    auto textNode = node.firstChild();
    string = textNode.nodeValue();
    return true;
}

bool EmploymentItemXmlLoader::load(const QDomNode &node,QString * const errmsg)
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

    }catch(const QString &str){
        if(errmsg)
            *errmsg = str;
        return false;
    }
    return true;
}

QDomNode EmploymentItemXmlLoader::save(QDomDocument &doc) const
{
    auto node = doc.createElement("employment");
    auto name = _item->name().split(" ");
    static const QString tagNames[]{"surname","name","middleName"};
    for(int i=0;i<3;++i){
        auto elem = doc.createElement(tagNames[i]);
        elem.appendChild(doc.createTextNode(name.at(i)));
        node.appendChild(elem);
    }
    for(int i=_item->metaObject()->propertyOffset();i<_item->metaObject()->propertyCount();++i){
        auto prop = _item->metaObject()->property(i);
        auto elem = doc.createElement(prop.name());
        elem.appendChild(doc.createTextNode(prop.read(_item).toString()));
        node.appendChild(elem);
    }
    return node;
}

