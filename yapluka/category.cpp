#include "category.h"


category::category()
{

}

category::category(QDomElement root, int level)
{
    level_ = level;
    qDebug()<<"on créer une catégorie";
    // Parcourir les éléments enfants de l'élément racine

    // Accéder à l'attribut "subject"
    QDomAttr subjectAttr = root.attributeNode("subject");
    if (!subjectAttr.isNull()) {
        name_ = subjectAttr.value();
        qDebug() << "Valeur de l'attribut 'subject' :" << subjectAttr.value();
    } else {
        qDebug() << "L'attribut 'subject' n'existe pas.";
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


    QDomNodeList rootElements = root.childNodes();
    for (int i = 0; i < rootElements.count(); ++i) {
        QDomNode node = rootElements.at(i);
        if (node.isElement() && node.toElement().tagName()=="category") {

            qDebug() << level<< " Nom de l'élément :" << node.toElement().tagName();
            category* item = new category(node.toElement(),level+1);
            children_.append(item);
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


list_category::list_category()
{
    master_ = new category();
}

void list_category::init(QString FileName)
{
    // Créer un document DOM
    QDomDocument document;

    // Charger le fichier XML
    QFile file(FileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Erreur lors de l'ouverture du fichier.");
        return;
    }

    // Parser le fichier XML
    if (!document.setContent(&file)) {
        qWarning("Erreur lors du parsing du fichier XML.");
        file.close();
        return;
    }
    file.close();

    // Obtenir l'élément racine
    QDomElement root = document.documentElement();
    if (root.isNull()) {
        qWarning("Le document XML est vide.");
        return;
    }

    master_ = new category(root);


}

void list_category::update_display(QTreeWidget* cat_widget)
{
    cat_widget->setHeaderLabels(QStringList() << "Nom" );
    master_->update_display(cat_widget->invisibleRootItem());
    cat_widget->expandAll();
}
