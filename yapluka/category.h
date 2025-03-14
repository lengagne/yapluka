#ifndef CATEGORY_H
#define CATEGORY_H

#include <QList>
#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QTreeWidget>


class category
{
public:
    category();

    category(QDomElement element, int level=0);

    category* get_cat_for_id( QString id);

    void update_display(QTreeWidgetItem* cat_widget);

    QList<QString> ids_;
    QString name_;
    int level_=0;
    QStringList bgColor_ = QStringList() << "255" << "255" << "255";
    QStringList fgColor_ = QStringList() << "0" << "0" << "0";

    QList<category*> children_;

};


#endif // CATEGORY_H
