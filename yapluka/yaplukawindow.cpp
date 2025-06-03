#include "yaplukawindow.h"
#include "./ui_yaplukawindow.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QSettings>
#include <QPushButton>

#include "task_dialog.h"


// to do, sauvegarder le nom de la colonne pour le tri (la c'est le numéro)

YaplukaWindow::YaplukaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::YaplukaWindow)
{
    ui->setupUi(this);

    setWindowTitle("Yapluka");

    QVBoxLayout * verticalLayout = new QVBoxLayout;
    QHBoxLayout * horizontalLayoutButton = new QHBoxLayout;

    horizontalLayoutButton->addWidget(ui->cachefinibox);
    horizontalLayoutButton->addWidget(ui->BoutonNouvelleTache);
    horizontalLayoutButton->addWidget(ui->BoutonEditTache);
    horizontalLayoutButton->addWidget(ui->BoutonFinirTache);
    horizontalLayoutButton->addWidget(ui->BoutonSupprimerTache);

    verticalLayout->addLayout(horizontalLayoutButton);

    QHBoxLayout * horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(ui->taskWidget);
    horizontalLayout->addWidget(ui->categorie_widget);

    verticalLayout->addLayout(horizontalLayout);

    // Assurez-vous que le centralwidget utilise ce layout
    ui->centralwidget->setLayout(verticalLayout);

    // Définissez le centralwidget comme widget central de la fenêtre principale
    this->setCentralWidget(ui->centralwidget);

    // Charger les paramètres au démarrage
    loadSettings();
    read_file();

    ui->taskWidget->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->taskWidget->header(), &QTreeWidget::customContextMenuRequested, this, &YaplukaWindow::showContextMenu);

    // Gérer l'ouverture d'une task
    connect(ui->taskWidget, &QTreeWidget::itemDoubleClicked, this, &YaplukaWindow::editTask);


    update_list();
}


YaplukaWindow::~YaplukaWindow()
{
    saveSettings();
    delete ui;
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
    for (int i = 0; i < 9; ++i) {
        headers.append(settings.value(QString::number(i), QString("Colonne %1").arg(i)).toString());
    }
    settings.endGroup();
    // Appliquer les en-têtes récupérés
    ui->taskWidget->setHeaderLabels(headers);

    // Restaurer la visibilité des colonnes
    settings.beginGroup("ColumnVisibility");
    for (int i = 0; i < 9; ++i) {
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

void YaplukaWindow::on_actionnouvelle_tache_triggered()
{
    task* new_task = new task();
    // Utilisez le constructeur approprié pour éditer une tâche existante
    task_dialog* dialog = new task_dialog(&categories_,new_task, this);
    connect(dialog, &task_dialog::accepted, this, &YaplukaWindow::updateTask);
    dialog->exec();
    tasks_.add_task(new_task);
    update_list();
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

