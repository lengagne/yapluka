#ifndef TASK_H
#define TASK_H

#include <QFile>
#include <QList>
#include <QString>
#include <QDebug>
#include <QListWidgetItem>
#include <QDateTime>
#include "list_category.h"



class task {

public:
    task();
    task(QDomElement element, int level=0);
    void update_category(list_category& cats);
    void update_display(QTreeWidgetItem* task_widget);

private:
    QString subject_;
    unsigned int priority_;
    QString id_;
    unsigned int percentage_;
    QList<task*> sub_tasks_;
    int level_=0;
    category* cat_ = nullptr;
    QString description_;
    QDateTime actualstartdate_, creationdate_,completiondate_,modificationdate_;
};

#endif
