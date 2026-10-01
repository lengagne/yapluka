#ifndef CATEGORY_H
#define CATEGORY_H

#include <QList>
#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QFont>
#include <QDebug>
#include <QDomDocument>
#include <QTreeWidget>
#include <QMetaType>

class category
{
public:
    category();

    category(QDomElement element, int level=0);

    void add_task_by_id(QString id);

    category* get_cat_for_id( QString id);

    category* get_cat_by_name( QString cat_name);

    void get_categories(QList<QString> & list);

    void remove_task_by_id(QString id);

    void save( QDomDocument& document, QDomElement& elroot);

    void update_display(QTreeWidgetItem* cat_widget);

    QList<QString> ids_;
    QString name_;
    int level_=0;
    QStringList bgColor_ = QStringList() << "255" << "255" << "255";
    QStringList fgColor_ = QStringList() << "0" << "0" << "0";

    QFont font_;

    QList<category*> children_;

};

Q_DECLARE_METATYPE(category*)

#endif // CATEGORY_H
