#include "category.h"


category::category()
{

}

category::category(QDomElement root, int level)
{
    level_ = level;
    // Parcourir les éléments enfants de l'élément racine

    // Accéder à l'attribut "subject"
    QDomAttr subjectAttr = root.attributeNode("subject");
    if (!subjectAttr.isNull()) {
        name_ = subjectAttr.value();
    } else {
    }

    QDomAttr bgcolorAttr = root.attributeNode("bgColor");
    if (!bgcolorAttr.isNull())
    {
        QString bgColorStr = bgcolorAttr.value();
        // Supprimer les parenthèses et diviser la chaîne en valeurs
        bgColorStr.remove("(").remove(")");
        bgColor_ = bgColorStr.split(", ");
    }
    QDomAttr fgcolorAttr = root.attributeNode("fgColor");
    if (!fgcolorAttr.isNull())
    {
        QString fgColorStr = fgcolorAttr.value();
        // Supprimer les parenthèses et diviser la chaîne en valeurs
        fgColorStr.remove("(").remove(")");
        fgColor_ = fgColorStr.split(", ");
    }

    QDomAttr idsAttr = root.attributeNode("categorizables");
    if (!idsAttr.isNull())
    {
        QString attrValue = idsAttr.value();
        QStringList idList = attrValue.split(" ",Qt::SplitBehaviorFlags::SkipEmptyParts);

        // Convertir QStringList en QList<QString>
        ids_ =  QList<QString>(idList.begin(), idList.end());
    }

    QDomNodeList rootElements = root.childNodes();
    for (int i = 0; i < rootElements.count(); ++i) {
        QDomNode node = rootElements.at(i);
        if (node.isElement() && node.toElement().tagName()=="category") {
            category* item = new category(node.toElement(),level+1);
            children_.append(item);
        }
    }
}

category* category::get_cat_for_id( QString id)
{
    if (ids_.contains(id)) {
        // Le QString est présent dans le QList
        //qDebug()<<" l'id "<<id <<" appartient à la cat: "<< name_;
        return this;
    } else {
        // Le QString n'est pas présent dans le QList
        for (category* c : children_)
        {
            if (c->get_cat_for_id(id))
            {
                return c->get_cat_for_id(id);
            }
        }
    }
    return 0;
}

category* category::get_cat_by_name( QString cat_name)
{
    if (name_ == cat_name)  return this;
    for (category* c : children_)
    {
        category* cat = c->get_cat_by_name(cat_name);
        if (cat)
        {
            return cat;
        }
    }
    return 0;
}

void category::get_categories(QList<QString> & list)
{
     for (category* c : children_)
     {
         list.append(c->name_);
         c->get_categories(list);
     }
}

void category::save(  QDomDocument& document,
                    QDomElement& elroot)
{
    if (level_)
    {
        QDomElement elcat = document.createElement("category");
        elcat.setAttribute("subject",name_);
        elcat.setAttribute("bgColor", "(" +bgColor_.join(", ") + ")");
        elcat.setAttribute("fgColor", "(" +fgColor_.join(", ") + ")");

        QString idsString;
        // Concaténer chaque élément de la liste en une seule chaîne
        for (int i = 0; i < ids_.size(); ++i) {
            idsString += ids_[i];
            // Ajouter un séparateur si ce n'est pas le dernier élément
            if (i < ids_.size() - 1) {
                idsString += " "; // Vous pouvez changer le séparateur selon vos besoins
            }
        }
        elcat.setAttribute("categorizables", idsString);


        for (category* c : children_)
        {
            c->save(document, elcat);
            elroot.appendChild(elcat);
        }

        elroot.appendChild(elcat);
    }else
    {
        for (category* c : children_)
        {
            c->save(document, elroot);
        }

    }


}

void category::update_display(QTreeWidgetItem* cat_widget)
{
    if (level_)
    {
        cat_widget->setText(0, name_);
        cat_widget->setBackground(0,QBrush(QColor(bgColor_[0].toInt(), bgColor_[1].toInt(), bgColor_[2].toInt())));
        cat_widget->setForeground(0,QBrush(QColor(fgColor_[0].toInt(), fgColor_[1].toInt(), fgColor_[2].toInt())));
    }
    //cat_widget->setBackground(0,QBrush(QColor(10,20,30))); // Jaune
    for (category* c : children_)
    {
        QTreeWidgetItem *childitem = new QTreeWidgetItem(cat_widget);
        c->update_display(childitem);
    }

  /*  QTreeWidgetItem *childItem1 = new QTreeWidgetItem(parentItem);
    childItem1->setText(0, "Enfant 1");
    childItem1->setBackground(0,QBrush(QColor(10,20,30))); // Jaune
    tree->expandAll();
*/

    //master_->update_display(cat_widget);
}

