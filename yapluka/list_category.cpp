#include "list_category.h"
#include <functional>

list_category::list_category()
{
    master_ = new category();
}

bool list_category::ajouter(QString nom)
{
    nom = nom.trimmed();

    // Refuser les noms vides ou déjà utilisés.
    if (nom.isEmpty() || master_->get_cat_by_name(nom))
        return false;

    auto *cat = new category();
    cat->name_ = nom;
    cat->level_ = 1;

    master_->children_.append(cat);
    return true;
}

bool list_category::supprimer(category *cat)
{
    if (!cat || cat == master_ || !cat->children_.isEmpty())
        return false;

    // Retrouver le parent, y compris pour une sous-catégorie.
    std::function<bool(category*)> retirer =
        [&](category *parent) -> bool {
            if (parent->children_.removeOne(cat)) {
                delete cat;
                return true;
            }

            for (category *enfant : parent->children_) {
                if (retirer(enfant))
                    return true;
            }

            return false;
        };

    return retirer(master_);
}

category* list_category::get_cat_for_id( QString id)
{
    return master_->get_cat_for_id( id);
}


void list_category::get_categories( QList<QString> & list)
{
    list = {""};
    master_->get_categories(list);
}

category* list_category::get_cat_by_name( QString cat_name)
{
    category* cat = master_->get_cat_by_name( cat_name);
    if (cat)    return cat;
    return master_;
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
