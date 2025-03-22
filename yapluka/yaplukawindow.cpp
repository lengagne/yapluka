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

    QVBoxLayout * verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(ui->cachefinibox);

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


void YaplukaWindow::on_actionOuvrir_triggered()
{
    currentFileName_ = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "", "Tous les fichiers (*.*)");
    read_file();
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


void YaplukaWindow::editTask(QTreeWidgetItem* item, int column) {
    if (item) {
        QString id = item->text(1);
        qDebug() << "looking for id " << id;
        task* task_to_edit = tasks_.get_task(id);

        // Utilisez le constructeur approprié pour éditer une tâche existante
        task_dialog* dialog = new task_dialog(task_to_edit, this);
        connect(dialog, &task_dialog::accepted, this, &YaplukaWindow::updateTask);
        dialog->exec();
    }
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
    qDebug()<<"YaplukaWindow::update_list()";
}


void YaplukaWindow::on_actionEnregistrer_triggered()
{
   // qDebug("On enregistre un fichier");
}


void YaplukaWindow::on_actionQuitter_triggered()
{
    QApplication::quit();
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

    settings.setValue("CacheFini", cache_fini_);
}

void YaplukaWindow::showContextMenu(const QPoint &pos) {
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


void YaplukaWindow::on_cachefinibox_stateChanged(int arg1)
{
    cache_fini_ = arg1;
    update_list();
}


void YaplukaWindow::on_actionnouvelle_tache_triggered()
{
    task* new_task = new task();
    tasks_.add_task(new_task);

    // Utilisez le constructeur approprié pour éditer une tâche existante
    task_dialog* dialog = new task_dialog(new_task, this);
    connect(dialog, &task_dialog::accepted, this, &YaplukaWindow::updateTask);
    dialog->exec();
    qDebug()<<"ajoute id = "<< new_task->id_;
    qDebug()<<"ajoute ptr = "<< new_task;
}

