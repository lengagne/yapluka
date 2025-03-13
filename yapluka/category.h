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

    void update_display(QTreeWidgetItem* cat_widget);

    QList<QString> ids_;
    QString name_;
    int level_=0;
    QStringList bgColor_ = QStringList() << "255" << "255" << "255";
    QStringList fgColor_ = QStringList() << "0" << "0" << "0";

    QList<category*> children_;
};


class list_category
{
public:
    list_category();

    void init(QString currentFileName_);

    void update_display(QTreeWidget* cat_widget);

private:
    category* master_;
};

#endif // CATEGORY_H
