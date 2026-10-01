#include "yaplukawindow.h"
#include "./ui_yaplukawindow.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QSettings>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QFontDialog>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QColor>
#include <QTimer>
#include <QLabel>
#include <QStatusBar>

#include "task_dialog.h"


// to do, sauvegarder le nom de la colonne pour le tri (la c'est le numéro)

YaplukaWindow::YaplukaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::YaplukaWindow)
{
    ui->setupUi(this);
    ui->taskWidget->setColumnCount(11);

    ui->categorie_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->categorie_widget,
            &QWidget::customContextMenuRequested,
            this,
            &YaplukaWindow::menuCategorie);



    setWindowTitle("Yapluka");

    QVBoxLayout * verticalLayout = new QVBoxLayout;
    QHBoxLayout * horizontalLayoutButton = new QHBoxLayout;

    horizontalLayoutButton->addWidget(ui->cachefinibox);
    horizontalLayoutButton->addWidget(ui->BoutonNouvelleTache);
    horizontalLayoutButton->addWidget(ui->BoutonEditTache);
    horizontalLayoutButton->addWidget(ui->BoutonFinirTache);
    horizontalLayoutButton->addWidget(ui->BoutonSupprimerTache);

    auto *boutonToutesCategories =
    new QPushButton(tr("Toutes les catégories"), this);
    horizontalLayoutButton->addWidget(boutonToutesCategories);

    connect(ui->categorie_widget, &QTreeWidget::itemClicked,
            this, [this](QTreeWidgetItem *item, int) {
        category_filter_ = item->text(0);
        apply_filter_category();
    });

    connect(boutonToutesCategories, &QPushButton::clicked,
            this, [this]() {
        category_filter_.clear();
        ui->categorie_widget->clearSelection();
        apply_filter_category();
    });


    auto *ajouterCat = new QPushButton(tr("+ Catégorie"), this);
    auto *supprimerCat = new QPushButton(tr("− Catégorie"), this);

    horizontalLayoutButton->addWidget(ajouterCat);
    horizontalLayoutButton->addWidget(supprimerCat);

    connect(ajouterCat, &QPushButton::clicked,
            this, &YaplukaWindow::ajouterCategorie);

    connect(supprimerCat, &QPushButton::clicked,
            this, &YaplukaWindow::supprimerCategorie);

    verticalLayout->addLayout(horizontalLayoutButton);

    QHBoxLayout * horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(ui->taskWidget);
    horizontalLayout->addWidget(ui->categorie_widget);

    verticalLayout->addLayout(horizontalLayout);

    // Assurez-vous que le centralwidget utilise ce layout
    ui->centralwidget->setLayout(verticalLayout);

    // Définissez le centralwidget comme widget central de la fenêtre principale
    this->setCentralWidget(ui->centralwidget);

    compteursLabel_ = new QLabel(this);

    compteursLabel_->setMargin(4);
    compteursLabel_->setToolTip(
        tr("Totaux de toutes les catégories, sous-tâches comprises. "
        "Les échéances sous 7 jours (retard inlus).")
    );

    statusBar()->addPermanentWidget(compteursLabel_, 1);

    // Charger les paramètres au démarrage
    loadSettings();
    read_file();

    ui->taskWidget->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->taskWidget->header(), &QTreeWidget::customContextMenuRequested, this, &YaplukaWindow::showContextMenu);

    // Gérer l'ouverture d'une task
    connect(ui->taskWidget, &QTreeWidget::itemDoubleClicked, this, &YaplukaWindow::editTask);


    update_list();

    auto *timerDeadline = new QTimer(this);

    connect(
        timerDeadline,
        &QTimer::timeout,
        this,
        [this, dernierJour = QDate::currentDate()]() mutable {
            QDate aujourdHui = QDate::currentDate();

            if (aujourdHui != dernierJour) {
                dernierJour = aujourdHui;
                update_list();
            }
        }
    );

    timerDeadline->start(60000);
}


YaplukaWindow::~YaplukaWindow()
{
    saveSettings();
    delete ui;
}

void YaplukaWindow::ajouterCategorie()
{
    bool ok = false;

    QString nom = QInputDialog::getText(
        this,
        tr("Ajouter une catégorie"),
        tr("Nom :"),
        QLineEdit::Normal,
        QString(),
        &ok
    ).trimmed();

    if (!ok || nom.isEmpty())
        return;

    if (!categories_.ajouter(nom)) {
        QMessageBox::information(
            this,
            tr("Catégorie"),
            tr("Une catégorie porte déjà ce nom.")
        );
        return;
    }

    update_list();
    save();
}


bool YaplukaWindow::filter_task(QTreeWidgetItem *item)
{
    bool correspond = category_filter_.isEmpty()
                      || item->text(3) == category_filter_;

    bool enfantVisible = false;

    for (int i = 0; i < item->childCount(); ++i) {
        // Toujours parcourir chaque enfant.
        if (filter_task(item->child(i))) {
            enfantVisible = true;
        }
    }

    bool visible = correspond || enfantVisible;
    item->setHidden(!visible);

    return visible;
}

void YaplukaWindow::apply_filter_category()
{
    // Éviter qu'une tâche devenue invisible reste sélectionnée.
    ui->taskWidget->clearSelection();
    ui->taskWidget->setCurrentItem(nullptr);

    for (int i = 0; i < ui->taskWidget->topLevelItemCount(); ++i) {
        filter_task(ui->taskWidget->topLevelItem(i));
    }

    ui->taskWidget->expandAll();
}

void YaplukaWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    // Créer des actions
    QAction *action_new_task = menu.addAction("Nouvelle Tâche");
    QAction *action_edit_task = menu.addAction("Editer Tâche");
    QAction *action_finish_task = menu.addAction("Finir Tâche");
    QAction *action_delete_task = menu.addAction("Supprimer Tâche");

    // Connecter les actions aux slots
    connect(action_new_task, &QAction::triggered, this, &YaplukaWindow::on_actionnouvelle_tache_triggered);
    connect(action_finish_task, &QAction::triggered, this, &YaplukaWindow::on_action_finish_tache_triggered);
    connect(action_edit_task, &QAction::triggered, this, &YaplukaWindow::onActionEdit);
    connect(action_delete_task, &QAction::triggered, this, &YaplukaWindow::onActionDeleteTask);

    // Afficher le menu contextuel
    menu.exec(event->globalPos());
}

void YaplukaWindow::editerCategorie()
{
    auto *item = ui->categorie_widget->currentItem();
    if (!item)
        return;

    category *cat = item->data(0, Qt::UserRole).value<category*>();
    if (!cat)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Éditer la catégorie"));

    auto *layout = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout;
    layout->addLayout(form);

    auto *nomEdit = new QLineEdit(cat->name_, &dialog);
    form->addRow(tr("Nom :"), nomEdit);

    auto *parentCombo = new QComboBox(&dialog);
    parentCombo->addItem(
        tr("(Aucune — à la racine)"),
        QVariant::fromValue(static_cast<category*>(nullptr))
    );

    category *parentActuel = categories_.parent_de(cat);

    for (category *candidate : categories_.toutes_categories()) {
        // Exclure la catégorie et tous ses descendants.
        bool interdit = false;

        for (category *p = candidate; p; p = categories_.parent_de(p)) {
            if (p == cat) {
                interdit = true;
                break;
            }
        }

        if (interdit)
            continue;

        // Afficher le chemin pour distinguer les catégories.
        QString chemin = candidate->name_;

        for (category *p = categories_.parent_de(candidate);
             p;
             p = categories_.parent_de(p)) {
            chemin.prepend(p->name_ + " / ");
        }

        parentCombo->addItem(chemin, QVariant::fromValue(candidate));

        if (candidate == parentActuel)
            parentCombo->setCurrentIndex(parentCombo->count() - 1);
    }

    form->addRow(tr("Catégorie parent :"), parentCombo);

    QFont police = cat->font_;

    auto *policeBouton = new QPushButton(tr("Choisir la police…"), &dialog);
    policeBouton->setFont(police);
    form->addRow(tr("Police :"), policeBouton);

    connect(policeBouton, &QPushButton::clicked, &dialog, [&]() {
        bool ok = false;

        QFont choix = QFontDialog::getFont(
            &ok, police, &dialog, tr("Police de la catégorie")
        );

        if (ok) {
            police = choix;
            policeBouton->setFont(police);
        }
    });

    // Couleurs temporaires : Annuler ne modifiera pas la catégorie.
    auto lireCouleur = [](const QStringList &valeurs,
                        const QColor &defaut) -> QColor {
        if (valeurs.size() < 3)
            return defaut;

        QColor couleur(
            valeurs[0].toInt(),
            valeurs[1].toInt(),
            valeurs[2].toInt()
        );

        return couleur.isValid() ? couleur : defaut;
    };

    QColor couleurFond = lireCouleur(cat->bgColor_, QColor(Qt::white));
    QColor couleurTexte = lireCouleur(cat->fgColor_, QColor(Qt::black));

    auto *fondBouton = new QPushButton(tr("Choisir…"), &dialog);
    auto *texteBouton = new QPushButton(tr("Choisir…"), &dialog);

    form->addRow(tr("Couleur du fond :"), fondBouton);
    form->addRow(tr("Couleur du texte :"), texteBouton);

    // Aperçu des deux couleurs sur le bouton de police existant.
    auto actualiserApercu = [&]() {
        fondBouton->setText(couleurFond.name());
        texteBouton->setText(couleurTexte.name());

        policeBouton->setStyleSheet(
            QString("QPushButton { background-color: %1; color: %2; }")
                .arg(couleurFond.name())
                .arg(couleurTexte.name())
        );
    };

    connect(fondBouton, &QPushButton::clicked, &dialog, [&]() {
        QColor choix = QColorDialog::getColor(
            couleurFond, &dialog, tr("Couleur du fond")
        );

        if (choix.isValid()) {
            couleurFond = choix;
            actualiserApercu();
        }
    });

    connect(texteBouton, &QPushButton::clicked, &dialog, [&]() {
        QColor choix = QColorDialog::getColor(
            couleurTexte, &dialog, tr("Couleur du texte")
        );

        if (choix.isValid()) {
            couleurTexte = choix;
            actualiserApercu();
        }
    });

    actualiserApercu();

    auto *boutons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        &dialog
    );

    layout->addWidget(boutons);

    connect(boutons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);

    connect(boutons, &QDialogButtonBox::accepted, &dialog, [&]() {
        QString nouveauNom = nomEdit->text().trimmed();

        if (nouveauNom.isEmpty()) {
            QMessageBox::warning(
                &dialog, tr("Nom invalide"),
                tr("Le nom ne peut pas être vide.")
            );
            return;
        }

        // L'éditeur de tâches retrouve les catégories par leur nom.
        // Éviter donc d'introduire un doublon lors du renommage.
        if (nouveauNom != cat->name_) {
            for (category *autre : categories_.toutes_categories()) {
                if (autre != cat && autre->name_ == nouveauNom) {
                    QMessageBox::warning(
                        &dialog, tr("Nom invalide"),
                        tr("Une catégorie porte déjà ce nom.")
                    );
                    return;
                }
            }
        }

        category *nouveauParent =
            parentCombo->currentData().value<category*>();

        if (!categories_.changer_parent(cat, nouveauParent)) {
            QMessageBox::warning(
                &dialog, tr("Parent invalide"),
                tr("Impossible de déplacer cette catégorie.")
            );
            return;
        }

        // Conserver le filtre si la catégorie filtrée est renommée.
        if (category_filter_ == cat->name_)
            category_filter_ = nouveauNom;

        cat->name_ = nouveauNom;
        cat->font_ = police;

        cat->bgColor_ = QStringList{
            QString::number(couleurFond.red()),
            QString::number(couleurFond.green()),
            QString::number(couleurFond.blue())
        };

        cat->fgColor_ = QStringList{
            QString::number(couleurTexte.red()),
            QString::number(couleurTexte.green()),
            QString::number(couleurTexte.blue())
        };

        dialog.accept();
    });

    if (dialog.exec() == QDialog::Accepted) {
        update_list();
        save();
    }
}

void YaplukaWindow::editTask(QTreeWidgetItem* item, int column) {
    if (item) {
        QString id = item->text(1);
        qDebug() << "looking for id " << id;
        task* task_to_edit = tasks_.get_task(id);

        // Utilisez le constructeur approprié pour éditer une tâche existante
        task_dialog* dialog = new task_dialog(&categories_,task_to_edit, this);
        connect(dialog, &task_dialog::accepted, this, &YaplukaWindow::updateTask);
        dialog->exec();
    }
    update_list();
    save();
}

void YaplukaWindow::loadSettings()
{
    ui->taskWidget->clear();
    QSettings settings("yapluka", "config");

    // Charger le nom du fichier
    currentFileName_ = settings.value("lastOpenedFile").toString();
    // Charger les noms des colonnes sauvegardés
    settings.beginGroup("ColumnNames");
    QStringList headers;
    // for (int i = 0; i < 9; ++i)
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i)
    {
        headers.append(settings.value(QString::number(i), QString("Colonne %1").arg(i)).toString());
    }
    settings.endGroup();
    // Appliquer les en-têtes récupérés
    ui->taskWidget->setHeaderLabels(headers);

    // Restaurer la visibilité des colonnes
    settings.beginGroup("ColumnVisibility");
    // for (int i = 0; i < 9; ++i)
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i)
    {
        bool visible = settings.value(QString::number(i), true).toBool();
        ui->taskWidget->setColumnHidden(i, !visible);
    }
    settings.endGroup();
    // Restaurer le tri
    if (settings.contains("Sort/Column") && settings.contains("Sort/Order")) {
        int column = settings.value("Sort/Column").toInt();
        Qt::SortOrder order = static_cast<Qt::SortOrder>(settings.value("Sort/Order").toInt());
        ui->taskWidget->sortItems(column, order);
    }
    cache_fini_ = settings.value("CacheFini").toBool();

    window_size_ = settings.value("windowSize", QSize(800, 600)).toSize();
    resize(window_size_);

    for (int i = 0; i < ui->taskWidget->columnCount(); ++i) {
        int width = settings.value(QString("columnWidth%1").arg(i), 100).toInt();
        ui->taskWidget->setColumnWidth(i, width);
    }

}

void YaplukaWindow::menuCategorie(const QPoint &pos)
{
    auto *item = ui->categorie_widget->itemAt(pos);
    if (!item)
        return;

    // Les actions doivent porter sur la ligne cliquée.
    ui->categorie_widget->setCurrentItem(item);

    QMenu menu(this);

    QAction *editer = menu.addAction(tr("Éditer…"));
    QAction *supprimer = menu.addAction(tr("Supprimer la catégorie"));

    QAction *choix = menu.exec(
        ui->categorie_widget->viewport()->mapToGlobal(pos)
    );

    if (choix == editer)
        editerCategorie();
    else if (choix == supprimer)
        supprimerCategorie();
}

void YaplukaWindow::onActionDeleteTask() {
    qDebug()<<"On va supprimer une tache ";
    QTreeWidgetItem *item = ui->taskWidget->currentItem();

    if (item) {
        int ret = QMessageBox::warning(this, tr("Supprimer l'élément"),
                                       tr("Êtes-vous sûr de vouloir supprimer cet élément ?"),
                                       QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {

            QString id = item->text(1);

            task* task_to_delete = tasks_.get_task(id);
            qDebug() << "On va supprimer "<< task_to_delete->subject_;

            tasks_.delete_task(task_to_delete);
            update_list();
            save();
            //int row = treeWidget->indexOfTopLevelItem(item);
            //delete treeWidget->takeTopLevelItem(row);
        }
    } else {
        QMessageBox::information(this, tr("Suppression"), tr("Aucun élément sélectionné."));
    }
}

void YaplukaWindow::onActionEdit() {
    qDebug()<<"Action 2 triggered";
    QTreeWidgetItem *item = ui->taskWidget->currentItem();
    editTask(item,0);
    save();
}


void YaplukaWindow::on_actionEnregistrer_triggered()
{
   save();
}

void YaplukaWindow::on_actionEnresitrer_sous_triggered()
{
    qDebug("On enregistre un fichier");
    currentFileName_ = QFileDialog::getSaveFileName(this, "Enregistrer dans le fichier", "", "Tous les fichiers (*.tsk)");

    // Vérifier et ajouter l'extension .txt si nécessaire
    QFileInfo fileInfo(currentFileName_);
    QString suffix = fileInfo.suffix();
    if (suffix.isEmpty() || suffix.toLower() != "tsk") {
        currentFileName_.append(".tsk");
    }
    save();
}

void YaplukaWindow::on_actionOuvrir_triggered()
{
    currentFileName_ = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "", "Tous les fichiers (*.*)");
    read_file();
}

void YaplukaWindow::on_actionQuitter_triggered()
{
    save();
    QApplication::quit();
}

// void YaplukaWindow::on_actionnouvelle_tache_triggered()
// {
//     task* new_task = new task();
//     // Utilisez le constructeur approprié pour éditer une tâche existante
//     task_dialog* dialog = new task_dialog(&categories_,new_task, this);
//     connect(dialog, &task_dialog::accepted, this, &YaplukaWindow::updateTask);
//     dialog->exec();
//     tasks_.add_task(new_task);
//     update_list();
//     save();
// }
void YaplukaWindow::on_actionnouvelle_tache_triggered()
{
    task *nouvelle = new task();

    task_dialog dialog(&categories_, nouvelle, this, true);

    if (dialog.exec() != QDialog::Accepted) {
        delete nouvelle;
        return;
    }

    tasks_.add_task(nouvelle);
    update_list();
    save();
}

void YaplukaWindow::on_action_finish_tache_triggered()
{
    qDebug()<<"On va mettre une tâche termineée";
    int ret = QMessageBox::warning(this, tr("Finir la tâche"),
                                   tr("Êtes-vous sûr de vouloir finir cette tâche ?"),
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        QTreeWidgetItem *item = ui->taskWidget->currentItem();
        //editTask(item,0);
        QString id = item->text(1);
        qDebug() << "looking for id " << id;
        task* task_to_edit = tasks_.get_task(id);
        task_to_edit->completiondate_ = QDateTime::currentDateTime();
        task_to_edit->percentage_ = 100;
        task_to_edit->status_ = 1;
    }
    update_list();
    save();
}


void YaplukaWindow::on_cachefinibox_stateChanged(int arg1)
{
    cache_fini_ = arg1;
    update_list();
}

void YaplukaWindow::read_file()
{
    category_filter_.clear();
    if (!currentFileName_.isEmpty()) {
        //QMessageBox::information(this, "Fichier sélectionné", currentFileName_);
    }

    categories_.init(currentFileName_);
    tasks_.init(currentFileName_,categories_);
    update_list();
}

void YaplukaWindow::save()
{
   qDebug()<<"on va enregistrer dans :"<< currentFileName_;

   // Créer un document XML
   QDomDocument document;
   // Ajouter l'en-tête XML
   QDomProcessingInstruction xmlHeader = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
   document.appendChild(xmlHeader);

//   // Ajouter l'instruction de traitement pour taskcoach
//   QDomProcessingInstruction taskcoachHeader = document.createProcessingInstruction("taskcoach", "release=\"1.4.6\" tskversion=\"37\"");
//   document.appendChild(taskcoachHeader);

   QDomElement eltasks = document.createElement("tasks");
   document.appendChild(eltasks);

   tasks_.save(document,eltasks);
   categories_.save(document,eltasks);

   // Enregistrer le document XML dans un fichier
   QFile file(currentFileName_);
   if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
       QTextStream stream(&file);
       stream << document.toString();
       file.close();
   } else {
       QMessageBox::warning(nullptr, "Erreur", "Impossible d'ouvrir le fichier pour écrire.");
   }
}

void YaplukaWindow::saveSettings() const
{
    QSettings settings("yapluka", "config");

    // Sauvegarder le nom du fichier
    settings.setValue("lastOpenedFile", currentFileName_);


    // Sauvegarde des noms des colonnes
    settings.beginGroup("ColumnNames");
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i) {
        settings.setValue(QString::number(i), ui->taskWidget->headerItem()->text(i));
    }
    settings.endGroup();

    // Sauvegarde de la visibilité des colonnes
    settings.beginGroup("ColumnVisibility");
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i) {
        settings.setValue(QString::number(i), !ui->taskWidget->isColumnHidden(i));
    }
    settings.endGroup();

    // Sauvegarde du tri
    settings.setValue("Sort/Column", ui->taskWidget->sortColumn());
    settings.setValue("Sort/Order", static_cast<int>(ui->taskWidget->header()->sortIndicatorOrder()));

    // Sauvegarde de la largeur des colonnes
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i) {
        settings.setValue(QString("columnWidth%1").arg(i), ui->taskWidget->columnWidth(i));
    }

    settings.setValue("CacheFini", cache_fini_);

    settings.setValue("windowSize", size());
}


void YaplukaWindow::showContextMenu(const QPoint &pos)
{
    // Créer un menu contextuel
    QMenu contextMenu(tr("Context menu"), this);

    // Ajouter des actions pour chaque colonne
    for (int i = 0; i < ui->taskWidget->columnCount(); ++i) {

        QTreeWidgetItem *headerItem = ui->taskWidget->headerItem();

        QAction *action = new QAction(headerItem->text(i), this);
        action->setCheckable(true);
        action->setChecked(!ui->taskWidget->isColumnHidden(i));
        connect(action, &QAction::triggered, [this, i](bool checked) {
            ui->taskWidget->setColumnHidden(i, !checked);
        });
        contextMenu.addAction(action);
    }

    // Afficher le menu contextuel
    contextMenu.exec(ui->taskWidget->viewport()->mapToGlobal(pos));
}


void YaplukaWindow::supprimerCategorie()
{
    auto *item = ui->categorie_widget->currentItem();

    if (!item) {
        QMessageBox::information(
            this, tr("Catégorie"),
            tr("Sélectionnez une catégorie à supprimer.")
        );
        return;
    }

    category *cat = item->data(0, Qt::UserRole).value<category*>();

    if (!cat)
        return;

    if (!cat->children_.isEmpty()) {
        QMessageBox::information(
            this, tr("Catégorie"),
            tr("Supprimez d'abord les sous-catégories.")
        );
        return;
    }

    auto reponse = QMessageBox::question(
        this,
        tr("Supprimer une catégorie"),
        tr("Supprimer « %1 » ?\n"
           "Les tâches seront conservées sans cette catégorie.")
            .arg(cat->name_),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reponse != QMessageBox::Yes)
        return;

    if (!categories_.supprimer(cat))
        return;

    // Réparer les liens avant de reconstruire l'affichage.
    tasks_.actualiser_categories(categories_);

    // Si vous avez ajouté le filtre de la réponse précédente :
    category_filter_.clear();

    update_list();
    save();
}


void YaplukaWindow::updateTask( ) {
    update_list();
}

void YaplukaWindow::update_list()
{
    ui->taskWidget->clear();
    tasks_.update_display(ui->taskWidget,cache_fini_);

    ui->categorie_widget->clear();
    categories_.update_display(ui->categorie_widget);

    ui->cachefinibox->setChecked(cache_fini_);

    apply_filter_category();

    const auto compteurs = tasks_.compter();

    compteursLabel_->setText(
        tr("Achevées : %1    |    En cours : %2    |    "
        "À faire sous 7 jours : %3")
            .arg(compteurs.achevees)
            .arg(compteurs.enCours)
            .arg(compteurs.sousSeptJours)
    );
}


void YaplukaWindow::on_BoutonNouvelleTache_clicked()
{
    on_actionnouvelle_tache_triggered();
}


void YaplukaWindow::on_BoutonEditTache_clicked()
{
    onActionEdit();
}


void YaplukaWindow::on_BoutonFinirTache_clicked()
{
    on_action_finish_tache_triggered();
}


void YaplukaWindow::on_BoutonSupprimerTache_clicked()
{
    onActionDeleteTask();
}

