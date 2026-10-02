#include "task.h"
#include <QApplication>

QString generateUniqueId() {
    // Obtenez la date et l'heure actuelles
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString(Qt::ISODate);

    // Combinez les informations pour créer une chaîne unique
    QString uniqueString = dateTimeString;

    // Utilisez un hachage pour générer un UUID basé sur cette chaîne
    QByteArray hash = QCryptographicHash::hash(uniqueString.toUtf8(), QCryptographicHash::Sha1);

    // Tronquez le hachage à 16 octets
    if (hash.size() > 16) {
        hash = hash.left(16);
    }
    return hash.toHex();
}


task::task()
{
    id_ = generateUniqueId();

    const QDateTime maintenant = QDateTime::currentDateTime();

    creationdate_ = maintenant;
    modificationdate_ = maintenant;

    percentage_ = 0;
    priority_ = 0;
    status_ = 0;
}

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

    QDomAttr statusAttr = root.attributeNode("status");
    if (!statusAttr.isNull()) {
        status_ = statusAttr.value().toInt();
    } else {
        status_ = 0;
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
            QDomElement ele = node.toElement();
            description_ = ele.text();
        }

    }

    QString valeur = root.attribute("deadline", "-1");

    deadline_ = valeur == "-1"
        ? QDate()
        : QDate::fromString(valeur, Qt::ISODate);
}

void task::delete_task(task* t)
{
    sub_tasks_.removeOne(t);
    for (task* c : sub_tasks_)
    {
        c->delete_task(t);
    }
}

task* task::get_task( QString id)
{
    if (id == id_)
        return this;
    task* out = nullptr;
    for (task* c : sub_tasks_)
    {
        out = c->get_task(id);
        if (out)
            return out;
    }
    return nullptr;
}

void task::save(  QDomDocument& document,
                  QDomElement& elroot)
{
    if (level_)
    {
        elroot.setAttribute("actualstartdate",actualstartdate_.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        elroot.setAttribute("completiondate",completiondate_.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        elroot.setAttribute("creationDateTime",creationdate_.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        elroot.setAttribute("modificationDateTime",modificationdate_.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        elroot.setAttribute("id",id_);
        elroot.setAttribute("percentageComplete",percentage_);
        elroot.setAttribute("priority",priority_);
        elroot.setAttribute("status",status_);
        elroot.setAttribute("subject",subject_);
        QDomElement eldescription = document.createElement("description");
        QDomText text1 = document.createTextNode(description_);
        eldescription.appendChild(text1);
        elroot.appendChild(eldescription);

        elroot.setAttribute(
            "deadline",
            deadline_.isValid()
                ? deadline_.toString(Qt::ISODate)
                : QStringLiteral("-1")
        );
    }
    for (task* c : sub_tasks_)
    {
        QDomElement eltask = document.createElement("task");
        c->save(document, eltask);
        elroot.appendChild(eltask);
    }

}

void task::update_category(list_category& cats)
{
    cat_ = cats.get_cat_for_id(id_);
    for (task* c : sub_tasks_)
        c->update_category(cats);
}

void task::update(const QList<QLineEdit*>& editFields)
{
    qDebug()<<"previous value of titre : "<< subject_;
//        for (int i = 0; i < editFields.size(); ++i) {
//            item->setText(i, editFields[i]->text());
//        }
    qDebug()<<"il faut mettre à jour l'item id : "<< id_ ;

}
/*
void task::update_display(QTreeWidgetItem* task_widget, bool cache)
{
    if ( percentage_<99 || !cache)
    {
        if (level_ )
        {

            // Stocker la priorité dans l'élément
            task_widget->setData(0, Qt::UserRole + 1, percentage_ != 0);

            // Assurez-vous que le QTreeWidget est accessible ici
            QTreeWidget *treeWidget = task_widget->treeWidget();
            if (treeWidget)
            {
                // Appliquer le délégué à la colonne entière
                CircleDelegate *circleDelegate = new CircleDelegate( percentage_ !=0 );
                treeWidget->setItemDelegateForColumn(0, circleDelegate);
            }

            task_widget->setText(1, id_);
            task_widget->setText(2, subject_);
            if (cat_)
                task_widget->setText(3, cat_->name_);
            else
                task_widget->setText(3, "");

            task_widget->setText(4, QString::asprintf("%02d",priority_));
            task_widget->setText(5, creationdate_.toString("yyyy-MM-dd"));
            task_widget->setText(6, actualstartdate_.toString("yyyy-MM-dd"));
            task_widget->setText(7, completiondate_.toString("yyyy-MM-dd"));
            task_widget->setText(8, modificationdate_.toString("yyyy-MM-dd"));

            task_widget->setData(9, Qt::UserRole + 2, percentage_ );
            ProgressBarDelegate *progressDelegate = new ProgressBarDelegate();
            treeWidget->setItemDelegateForColumn(9, progressDelegate);
            task_widget->setText(9, QString::asprintf("%03d",percentage_));

            task_widget->setTextAlignment(4, Qt::AlignCenter);
            task_widget->setTextAlignment(9, Qt::AlignCenter);

            if (cat_)
            {
                for (int i=0;i<11;i++)
                {
                    task_widget->setFont(i, cat_->font_);
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
}*/

void task::update_display(QTreeWidgetItem *parentItem, bool cache)
{
    QTreeWidget *tree = parentItem->treeWidget();
    if (!tree)
        return;

    QTreeWidgetItem *parentEnfants = parentItem;

    bool terminee = percentage_ >= 100;

    // level_ == 0 correspond à la racine technique.
    if (level_ != 0 && (!cache || !terminee)) {
        int urgence = 2; // 0 : retard, 1 : semaine, 2 : autres

        if (!terminee && deadline_.isValid()) {
            qint64 jours = QDate::currentDate().daysTo(deadline_);

            if (jours < 0)
                urgence = 0;
            else if (jours <= 7)
                urgence = 1;
        }

        // Les tâches urgentes sont visibles au premier niveau.
        QTreeWidgetItem *destination =
            urgence < 2
                ? tree->invisibleRootItem()
                : parentItem;

        auto *item = new TaskTreeItem(destination);

        item->setData(0, TaskTreeItem::UrgenceRole, urgence);
        item->setData(0, Qt::UserRole + 1, percentage_ != 0);

        item->setText(1, id_);
        item->setText(2, subject_);
        item->setText(3, cat_ ? cat_->name_ : QString());

        item->setText(
            4, QString::number(priority_).rightJustified(2, '0')
        );

        const QString format = QStringLiteral("yyyy-MM-dd HH:mm");

        item->setText(5, creationdate_.toString(format));
        item->setText(6, actualstartdate_.toString(format));
        item->setText(7, completiondate_.toString(format));
        item->setText(8, modificationdate_.toString(format));

        item->setData(9, Qt::UserRole + 2, percentage_);
        item->setText(
            9, QString::number(percentage_).rightJustified(3, '0')
        );

        item->setText(
            10,
            deadline_.isValid()
                ? deadline_.toString(Qt::ISODate)
                : QStringLiteral("—")
        );

        item->setTextAlignment(4, Qt::AlignCenter);
        item->setTextAlignment(9, Qt::AlignCenter);
        item->setTextAlignment(10, Qt::AlignCenter);

        // Couleurs habituelles de la catégorie.
        if (cat_) {
            QColor fond(
                cat_->bgColor_[0].toInt(),
                cat_->bgColor_[1].toInt(),
                cat_->bgColor_[2].toInt()
            );

            QColor texte(
                cat_->fgColor_[0].toInt(),
                cat_->fgColor_[1].toInt(),
                cat_->fgColor_[2].toInt()
            );

            for (int i = 0; i < tree->columnCount(); ++i) {
                item->setBackground(i, QBrush(fond));
                item->setForeground(i, QBrush(texte));

                // Si vous avez ajouté font_ à category :
                item->setFont(i, cat_->font_);
            }
        }

        // L'urgence prend le dessus sur le style de catégorie
        // pour le titre et la deadline.
        if (urgence < 2) {
            QColor fond = urgence == 0
                ? QColor("#FEE2E2")
                : QColor("#FFEDD5");

            QColor texte = urgence == 0
                ? QColor("#991B1B")
                : QColor("#9A3412");

            QFont police = QApplication::font();
            police.setPointSize(urgence == 0 ? 22 : 16);
            police.setBold(true);

            for (int colonne : {2, 10}) {
                item->setFont(colonne, police);
                item->setBackground(colonne, QBrush(fond));
                item->setForeground(colonne, QBrush(texte));
            }

            QString indication;

            if (urgence == 0) {
                qint64 retard =
                    deadline_.daysTo(QDate::currentDate());

                indication =
                    QStringLiteral("En retard de %1 jour(s)")
                        .arg(retard);
            } else {
                indication =
                    QStringLiteral("À faire dans les 7 prochains jours");
            }

            item->setToolTip(2, indication);
            item->setToolTip(10, indication);
        }

        parentEnfants = item;
    }

    // Parcourir aussi les enfants d'une tâche terminée masquée :
    // une sous-tâche inachevée doit rester accessible.
    for (task *enfant : sub_tasks_)
        enfant->update_display(parentEnfants, cache);
}
