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

QList<category*> list_category::toutes_categories() const
{
    QList<category*> resultat;

    std::function<void(category*)> parcourir = [&](category *parent) {
        for (category *enfant : parent->children_) {
            resultat.append(enfant);
            parcourir(enfant);
        }
    };

    parcourir(master_);
    return resultat;
}

category* list_category::parent_de(category *cat) const
{
    std::function<category*(category*)> chercher =
        [&](category *parent) -> category* {
            for (category *enfant : parent->children_) {
                if (enfant == cat)
                    return parent;

                if (category *resultat = chercher(enfant))
                    return resultat;
            }

            return nullptr;
        };

    category *parent = chercher(master_);
    return parent == master_ ? nullptr : parent;
}

bool list_category::changer_parent(category *cat,
                                  category *nouveauParent)
{
    const auto categories = toutes_categories();

    if (!cat || !categories.contains(cat))
        return false;

    if (nouveauParent && !categories.contains(nouveauParent))
        return false;

    // Interdire de déplacer une catégorie sous elle-même
    // ou sous l'un de ses descendants.
    for (category *p = nouveauParent; p; p = parent_de(p)) {
        if (p == cat)
            return false;
    }

    category *ancienParent = parent_de(cat);
    if (!ancienParent)
        ancienParent = master_;

    category *destination = nouveauParent ? nouveauParent : master_;

    if (ancienParent == destination)
        return true;

    if (!ancienParent->children_.removeOne(cat))
        return false;

    destination->children_.append(cat);

    // L'affichage et la sauvegarde utilisent level_.
    std::function<void(category*, int)> actualiserNiveau =
        [&](category *element, int niveau) {
            element->level_ = niveau;

            for (category *enfant : element->children_)
                actualiserNiveau(enfant, niveau + 1);
        };

    actualiserNiveau(cat, destination->level_ + 1);
    return true;
}

void list_category::update_display(QTreeWidget* cat_widget)
{
    cat_widget->setHeaderLabels(QStringList() << "Nom" );
    master_->update_display(cat_widget->invisibleRootItem());
    cat_widget->expandAll();
}
