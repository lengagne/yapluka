
#ifndef LIST_CATEGORY_H
#define LIST_CATEGORY_H

#include <category.h>

class list_category
{
public:
    list_category();

    category* get_cat_for_id( QString id);

    void get_categories( QList<QString> & list);

    void init(QString currentFileName_);

    void save( QDomDocument& document, QDomElement& elroot);

    void update_display(QTreeWidget* cat_widget);

private:
    category* master_;
};

#endif
