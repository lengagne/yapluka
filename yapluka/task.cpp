#include "task.h"


task::task(QDomElement root, int level)
{
    level_ = level;
    // Parcourir les éléments enfants de l'élément racine

    // Accéder à l'attribut "subject"
    QDomAttr subjectAttr = root.attributeNode("subject");
    if (!subjectAttr.isNull()) {
        subject_ = subjectAttr.value();
    } else {
        subject_ = "not defined";
    }

    QDomAttr idAttr = root.attributeNode("id");
    if (!subjectAttr.isNull()) {
        id_ = idAttr.value();
    } else {
        id_ = "not defined";
    }

    QDomAttr priorityAttr = root.attributeNode("priority");
    if (!priorityAttr.isNull()) {
        priority_ = priorityAttr.value().toInt();
    } else {
        priority_ = 0;
    }

    QDomAttr percentageAttr = root.attributeNode("percentageComplete");
    if (!percentageAttr.isNull()) {
        percentage_ = percentageAttr.value().toInt();
    } else {
        percentage_ = 0;
    }


    QDomAttr creationdateAttr = root.attributeNode("creationDateTime");
    if (!creationdateAttr.isNull()) {
        QString date =  creationdateAttr.value().left(23);;
        creationdate_ = QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss.zzz");
    } else {
    }


    QDomAttr startdateAttr = root.attributeNode("actualstartdate");
    if (!startdateAttr.isNull()) {
        QString date =  startdateAttr.value().left(23);;
        actualstartdate_ = QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss.zzz");
    } else {
    }

    QDomAttr completiondateAttr = root.attributeNode("completiondate");
    if (!completiondateAttr.isNull()) {
        QString date =  completiondateAttr.value().left(23);;
        completiondate_ = QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss.zzz");
    } else {
    }

    QDomAttr modifdateAttr = root.attributeNode("modificationDateTime");
    if (!modifdateAttr.isNull()) {
        QString date =  modifdateAttr.value().left(23);;
        modificationdate_ = QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss.zzz");
    } else {
    }



    QDomNodeList rootElements = root.childNodes();
    for (int i = 0; i < rootElements.count(); ++i) {
        QDomNode node = rootElements.at(i);
        if (node.isElement() && node.toElement().tagName()=="task") {
            task* item = new task(node.toElement(),level+1);
            sub_tasks_.append(item);
        }else if (node.isElement() && node.toElement().tagName()=="description") {
            description_ = node.toElement().toText().data();
        }
    }
}

void task::update_category(list_category& cats)
{
    cat_ = cats.get_cat_for_id(id_);
    for (task* c : sub_tasks_)
        c->update_category(cats);
}

void task::update_display(QTreeWidgetItem* task_widget, bool cache)
{
    if ( percentage_<99 || !cache)
    {
        if (level_ )
        {
            task_widget->setText(0, id_);
            task_widget->setText(1, subject_);
            if (cat_)
                task_widget->setText(2, cat_->name_);
            else
                task_widget->setText(2, "");

            task_widget->setText(3, QString::asprintf("%02d",priority_));
            task_widget->setText(4, creationdate_.toString("yyyy-MM-dd"));
            task_widget->setText(5, actualstartdate_.toString("yyyy-MM-dd"));
            task_widget->setText(6, completiondate_.toString("yyyy-MM-dd"));
            task_widget->setText(7, modificationdate_.toString("yyyy-MM-dd"));
            task_widget->setText(8, QString::asprintf("%03d",percentage_));

            if (cat_)
            {
                for (int i=0;i<9;i++)
                {
                    task_widget->setBackground(i,QBrush(QColor(cat_->bgColor_[0].toInt(), cat_->bgColor_[1].toInt(), cat_->bgColor_[2].toInt())));
                    task_widget->setForeground(i,QBrush(QColor(cat_->fgColor_[0].toInt(), cat_->fgColor_[1].toInt(), cat_->fgColor_[2].toInt())));
                }
            }

        }
        for (task* c : sub_tasks_)
        {
            if ( c->percentage_<99 || !cache)
            {
                QTreeWidgetItem *childitem = new QTreeWidgetItem(task_widget);
                c->update_display(childitem,cache);
            }
        }
    }
}
