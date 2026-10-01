
#ifndef LIST_CATEGORY_H
#define LIST_CATEGORY_H

#include <category.h>

class list_category
{
public:
    list_category();

    category* get_cat_for_id( QString id);

    category* get_cat_by_name( QString cat_name);

    bool ajouter(QString nom);

    bool supprimer(category *cat);

    void get_categories( QList<QString> & list);

    void init(QString currentFileName_);

    void save( QDomDocument& document, QDomElement& elroot);

    void update_display(QTreeWidget* cat_widget);

    QList<category*> toutes_categories() const;

    category* parent_de(category *cat) const;

    bool changer_parent(category *cat, category *nouveauParent);

private:
    category* master_;
};

#endif
