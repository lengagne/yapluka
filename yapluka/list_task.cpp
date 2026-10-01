#include "list_task.h"
#include <functional>
#include <QDate>

list_task::Compteurs list_task::compter() const
{
    Compteurs resultat;

    if (!master_)
        return resultat;

    const QDate aujourdHui = QDate::currentDate();

    std::function<void(const task*)> parcourir =
        [&](const task *t) {
            // Ne pas compter la racine technique.
            if (t != master_) {
                if (t->percentage_ >= 100) {
                    ++resultat.achevees;
                } else {
                    ++resultat.enCours;

                    if (t->deadline_.isValid()) {
                        qint64 jours =
                            aujourdHui.daysTo(t->deadline_);

                        if ( jours <= 7)
                            ++resultat.sousSeptJours;
                    }
                }
            }

            for (const task *enfant : t->sub_tasks_)
                parcourir(enfant);
        };

    parcourir(master_);
    return resultat;
}

task* list_task::get_task( QString id)
{
    return master_->get_task(id);
}

void list_task::init(QString FileName,
                     list_category& categories_)
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

    master_ = new task(root);
    master_->update_category(categories_);

}

void list_task::save( QDomDocument& document,
                      QDomElement& elroot)
{
    master_->save(document,elroot);
}

// void list_task::update_display(QTreeWidget* task_widget,
//                                 bool cache)
// {
//     task_widget->setHeaderLabels(QStringList() << "status"<<"id"<<"tache"<<"categorie"<<"priorite"<<"date création"<<"date début"<<"date complétion"<<"date modif"<<"%" );
//     if (master_)
//         master_->update_display(task_widget->invisibleRootItem(),cache);
//     task_widget->expandAll();
//
//     // Activation du tri par colonne
//     task_widget->setSortingEnabled(true);
//
//     QTreeWidgetItem *headerItem = task_widget->headerItem();
//     headerItem->setTextAlignment(4, Qt::AlignCenter);
//     headerItem->setTextAlignment(9, Qt::AlignCenter);
// }
void list_task::update_display(QTreeWidget *task_widget, bool cache)
{
    // Ne pas trier pendant la construction des lignes :
    // leurs données d'urgence ne sont pas encore toutes renseignées.
    task_widget->setSortingEnabled(false);

    task_widget->setHeaderLabels(QStringList()
        << "status"
        << "id"
        << "tache"
        << "categorie"
        << "priorite"
        << "date création"
        << "date début"
        << "date complétion"
        << "date modif"
        << "%"
        << "Deadline"
    );

    // Installer les délégués une seule fois.
    if (!dynamic_cast<CircleDelegate*>(
            task_widget->itemDelegateForColumn(0))) {
        auto *delegate = new CircleDelegate(0);
        delegate->setParent(task_widget);
        task_widget->setItemDelegateForColumn(0, delegate);
    }

    if (!dynamic_cast<ProgressBarDelegate*>(
            task_widget->itemDelegateForColumn(9))) {
        auto *delegate = new ProgressBarDelegate();
        delegate->setParent(task_widget);
        task_widget->setItemDelegateForColumn(9, delegate);
    }

    // Nécessaire pour des lignes de hauteurs différentes.
    task_widget->setUniformRowHeights(false);

    if (master_)
        master_->update_display(
            task_widget->invisibleRootItem(), cache
        );

    task_widget->setSortingEnabled(true);
    task_widget->expandAll();

    auto *header = task_widget->headerItem();
    header->setTextAlignment(4, Qt::AlignCenter);
    header->setTextAlignment(9, Qt::AlignCenter);
    header->setTextAlignment(10, Qt::AlignCenter);
}
