#include "list_task.h"


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
    qDebug()<<"list_task->save";
    master_->save(document,elroot);

}

void list_task::update_display(QTreeWidget* task_widget,
                                bool cache)
{
    task_widget->setHeaderLabels(QStringList() << "status"<<"id"<<"tache"<<"categorie"<<"priorite"<<"date création"<<"date début"<<"date complétion"<<"date modif"<<"%" );
    if (master_)
        master_->update_display(task_widget->invisibleRootItem(),cache);
    task_widget->expandAll();

    // Activation du tri par colonne
    task_widget->setSortingEnabled(true);
}
