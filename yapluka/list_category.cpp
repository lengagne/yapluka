#include "list_category.h"

list_category::list_category()
{
    master_ = new category();
}

category* list_category::get_cat_for_id( QString id)
{
    return master_->get_cat_for_id( id);
}


void list_category::get_categories( QList<QString> & list)
{
    list = {"","Banana"};
    master_->get_categories(list);
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

void list_category::save( QDomDocument& document,
                          QDomElement& elroot)
{
    master_->save(document,elroot);
}

void list_category::update_display(QTreeWidget* cat_widget)
{
    cat_widget->setHeaderLabels(QStringList() << "Nom" );
    master_->update_display(cat_widget->invisibleRootItem());
    cat_widget->expandAll();
}
