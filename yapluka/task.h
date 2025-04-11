#ifndef TASK_H
#define TASK_H

#include <QFile>
#include <QList>
#include <QString>
#include <QDebug>
#include <QListWidgetItem>
#include <QDateTime>

#include <QStyledItemDelegate>
#include <QPainter>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDebug>


#include "list_category.h"

QString generateUniqueId();

class CircleDelegate : public QStyledItemDelegate {
public:

    CircleDelegate( int status)
    {
        status_ = status;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {

        // Vérifier le rôle personnalisé pour la priorité
        int status = index.data(Qt::UserRole + 1).toInt();

        // Choisir la couleur en fonction de la priorité
        QColor color = (status) ? Qt::blue : Qt::gray;

        // Appeler la méthode de base pour dessiner le reste de l'élément
        QStyledItemDelegate::paint(painter, option, index);
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(option.rect.center(), 10, 10);
    }

private :
    int status_;
};

class ProgressBarDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        // Appeler la méthode de base pour dessiner le reste de l'élément
        QStyledItemDelegate::paint(painter, option, index);

        // Récupérer la valeur de progression depuis le modèle
        int progress = index.data(Qt::UserRole+2).toInt();

        // Dessiner la barre de progression
        QRect rect = option.rect;
        painter->setBrush(Qt::white);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect); // Fond blanc

        painter->setBrush(Qt::blue);
        painter->drawRect(rect.adjusted(0, 0, -rect.width() + rect.width() * progress / 100, 0));

        // Dessiner le texte de la progression
        painter->setPen(Qt::black);
        painter->drawText(rect, Qt::AlignCenter, QString("%1%").arg(progress));

    }
};


class task {

public:
    task();
    task(QDomElement element, int level=0);

    void add_sub_task( task* t)
    {
        t->level_ = level_+1;
        sub_tasks_.append(t);
        qDebug()<<"add_sub_task to task subject_ = "<< subject_;
        qDebug()<<"add_sub_task subject_ = "<< t->subject_;
    }

    task* get_task( QString id);

    void save(  QDomDocument& document,
                QDomElement& elroot);

    void update(const QList<QLineEdit*>& editFields);

    void update_category(list_category& cats);
    void update_display(QTreeWidgetItem* task_widget, bool cache=true);

private:
    QString subject_;
    unsigned int priority_;
    QString id_;
    int status_;
    unsigned int percentage_;
    QList<task*> sub_tasks_;
    int level_=0;
    category* cat_ = nullptr;
    QString description_;
    QDateTime actualstartdate_, creationdate_,completiondate_,modificationdate_;

    friend class task_dialog;
    friend class YaplukaWindow;
    friend class list_task;
};

#endif
