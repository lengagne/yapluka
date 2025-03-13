#include "yaplukawindow.h"
#include "./ui_yaplukawindow.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QSettings>


// to do, sauvegarder le nom de la colonne pour le tri (la c'est le numéro)

YaplukaWindow::YaplukaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::YaplukaWindow)
{
    for (int i=0;i<5;i++)
        print_option_[i] = true;

    ui->setupUi(this);

    // Charger les paramètres au démarrage
    loadSettings();
    read_file();
    qDebug()<<"currentFileName_ = "<< currentFileName_;

    // Vérifiez que le QTableWidget est bien ajouté à un layout
    QHBoxLayout * verticalLayout = new QHBoxLayout;
    verticalLayout->addWidget(ui->tableWidget);
    verticalLayout->addWidget(ui->categorie_widget);

    // Assurez-vous que le centralwidget utilise ce layout
    ui->centralwidget->setLayout(verticalLayout);

    // Définissez le centralwidget comme widget central de la fenêtre principale
    this->setCentralWidget(ui->centralwidget);

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
    qDebug("On ouvre un fichier");

    if (!currentFileName_.isEmpty()) {
        //QMessageBox::information(this, "Fichier sélectionné", currentFileName_);
    }

    TaskReader reader;
    reader.lireFichier(currentFileName_, tasks_);
    categories_.init(currentFileName_);

    update_list();
}

void YaplukaWindow::update_list()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QStringList headers;
    nb_colonnes_=0;
    for (int i=0;i<5;i++)
    {
//        qDebug()<<"print_option_["<< i<<"] = "<< print_option_[i];
        if( print_option_[i])
        {
            nb_colonnes_++;
            switch(i){
                case(0):    headers <<"id";   break;
                case(1):    headers <<"tache";   break;
                case(2):    headers <<"categorie";   break;
                case(3):    headers <<"priorite";   break;
                case(4):    headers <<"%";   break;
                default:    headers <<"tmp";
            }
        }
    }

    qDebug()<<"headers = "<< headers;
    ui->tableWidget->setColumnCount(nb_colonnes_);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->categorie_widget->clear();
    //ui->categorie_widget->setColumnCount(1);
    categories_.update_display(ui->categorie_widget);



/*    for (const category &c : categories_)
    {
        ui->categorie_widget->insertRow(0);

        QTableWidgetItem *item = new QTableWidgetItem(c.name_);
        if (item) {
            //qDebug()<<"c.bgColor_ = "<<c.bgColor_;
            item->setBackground(QBrush(QColor(c.bgColor_[0].toInt(), c.bgColor_[1].toInt(), c.bgColor_[2].toInt()))); // Jaune
        }

        ui->categorie_widget->setItem(0,0, item);
        qDebug()<<" print cat : "<< c.name_;
    }*/

    for (const Task &p : tasks_) if (affich_option_.affich_fini_ || p.percentageComplete <100)
    {
        ui->tableWidget->insertRow(0);
        int cpt =0;
        for (int i=0;i<5;i++)
        {
            if (print_option_[i])
              {
                  QTableWidgetItem *item = nullptr;
                  switch (i) {
                      case 0: item = new QTableWidgetItem(p.id); break;
                      case 1: item = new QTableWidgetItem(p.subject); break;
                      case 2: item = new QTableWidgetItem("Categorie"); break;
                      case 3:
                      {
                          item = new QTableWidgetItem();
                          item->setData(Qt::DisplayRole, p.priority);
                          item->setData(Qt::EditRole, p.priority);
                          break;
                      }
                      case 4:
                      {
                          //qDebug()<<"on affiche la priorité :"<<p.percentageComplete;
                          item = new QTableWidgetItem();
                          item->setData(Qt::DisplayRole, p.percentageComplete);
                          item->setData(Qt::EditRole, p.percentageComplete);
                          break;
                      }
                      default: qDebug() << "This should not happen";
                  }

                  if (item) {
                      ui->tableWidget->setItem(0, cpt++, item);
                  }
              }
        }
    }
}


void YaplukaWindow::on_actionEnregistrer_triggered()
{
    qDebug("On enregistre un fichier");
}


void YaplukaWindow::on_actionQuitter_triggered()
{
    qDebug("On quitte l'application");
    QApplication::quit();
}



void TaskReader::lireFichier(const QString &filePath,
                             QList<Task>& tasks)
{
    tasks.clear();
    //categories.clear();
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Erreur d'ouverture du fichier");
    }

    QXmlStreamReader xmlReader(&file);

    Task currentTask;
    category current_cat;

    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        // Si l'élément est une balise de début
        if (xmlReader.isStartElement()) {
            if (xmlReader.name() == "task") {
                currentTask = Task();  // Réinitialiser la structure
                if (xmlReader.attributes().hasAttribute("subject")) {
                    currentTask.subject = xmlReader.attributes().value("subject").toString();
                }
                if (xmlReader.attributes().hasAttribute("id")) {
                    currentTask.id = xmlReader.attributes().value("id").toString();
                }
                if (xmlReader.attributes().hasAttribute("priority")) {
                    currentTask.priority = xmlReader.attributes().value("priority").toInt();
                }
                if (xmlReader.attributes().hasAttribute("percentageComplete")) {
                    currentTask.percentageComplete = xmlReader.attributes().value("percentageComplete").toInt();
                }
            }
            /*else if (xmlReader.name() == "category"){
                current_cat = category();
                if (xmlReader.attributes().hasAttribute("subject")) {
                    current_cat.name_ = xmlReader.attributes().value("subject").toString();
                    qDebug()<<" found cat : "<< current_cat.name_;
                }
                if (xmlReader.attributes().hasAttribute("bgColor")) {
                    QString bgColorStr = xmlReader.attributes().value("bgColor").toString();
                    // Supprimer les parenthèses et diviser la chaîne en valeurs
                    bgColorStr.remove("(").remove(")");
                    current_cat.bgColor_ = bgColorStr.split(", ");
                }

            }*/
        }
        // Si l'élément est une balise de fin
        else if (xmlReader.isEndElement() && xmlReader.name() == "task") {
            tasks.append(currentTask);
        }
        /*else if (xmlReader.isEndElement() && xmlReader.name() == "category") {
            qDebug()<<" ajout de "<< current_cat.name_;
            categories.append(current_cat);
        }*/
    }

    if (xmlReader.hasError()) {
        qWarning("Erreur de parsing XML");
    }

    file.close();
    //return tasks;
}

void YaplukaWindow::on_treeView_collapsed(const QModelIndex &index)
{
    qDebug()<<"On doit collapser, mais je sais pas ce que c'est";

}

void YaplukaWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
 /*   qDebug()<<"On resize la fenetre  wdith ="<< this->width();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // S'assurer que le QMainWindow traite l'événement de redimensionnement

    qDebug()<<"nb_colonnes_ = "<<nb_colonnes_;
    for (int i=0;i<nb_colonnes_;i++)
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);*/
}

void YaplukaWindow::on_actionCacher_fini_triggered()
{
    affich_option_.affich_fini_ = false;
    qDebug()<<"on cache les fini !";
    update_list();
}


void YaplukaWindow::on_actionMontrer_fini_triggered()
{
    affich_option_.affich_fini_ = true;
    qDebug()<<"on montre les fini !";
    update_list();
}




void YaplukaWindow::on_actionid_triggered()
{
    print_option_[0] = !print_option_[0];
    update_list();
}


void YaplukaWindow::on_actiontache_2_triggered()
{
    print_option_[1] = !print_option_[1];
    update_list();
}


void YaplukaWindow::on_actioncategorie_triggered()
{
    print_option_[2] = !print_option_[2];
    update_list();
}


void YaplukaWindow::on_actionpriorite_triggered()
{
    print_option_[3] = !print_option_[2];
    update_list();
}


void YaplukaWindow::on_actionpourcentage_triggered()
{
    print_option_[4] = !print_option_[4];
    update_list();
}

void YaplukaWindow::loadSettings()
{
    QSettings settings("yapluka", "config");

    // Charger le nom du fichier
    currentFileName_ = settings.value("lastOpenedFile").toString();

    // Charger les configurations des cases à cocher
    int size = settings.beginReadArray("printOptions");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        print_option_[i] = settings.value("checked").toBool();
    }
    settings.endArray();

    // Charger l'option d'affichage des tâches terminées
    affich_option_.affich_fini_ = settings.value("showFinishedTasks", true).toBool();

    // Charger l'état de tri
    int sortColumn = settings.value("sortColumn", -1).toInt();
    Qt::SortOrder sortOrder = static_cast<Qt::SortOrder>(settings.value("sortOrder", Qt::AscendingOrder).toInt());

    // Appliquer l'état de tri si une colonne valide est trouvée
    if (sortColumn >= 0 && sortColumn < ui->tableWidget->columnCount()) {
        ui->tableWidget->sortItems(sortColumn, sortOrder);
    }

    // on remet à jour les check box
    ui->actionid->setChecked(print_option_[0]);
    update_list();

    if (sortColumn >= 0 && sortColumn < ui->tableWidget->columnCount()) {
        ui->tableWidget->sortItems(sortColumn, sortOrder);
    }

}

void YaplukaWindow::saveSettings() const
{
    qDebug()<<"saveSettings()";
    QSettings settings("yapluka", "config");

    // Sauvegarder le nom du fichier
    settings.setValue("lastOpenedFile", currentFileName_);

    // Sauvegarder les configurations des cases à cocher
    settings.beginWriteArray("printOptions");
    for (int i = 0; i < 5; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("checked", print_option_[i]);
    }
    settings.endArray();

    // Sauvegarder l'option d'affichage des tâches terminées
    settings.setValue("showFinishedTasks", affich_option_.affich_fini_);

    // Obtenir l'état de tri actuel
    int sortColumn = ui->tableWidget->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder sortOrder = ui->tableWidget->horizontalHeader()->sortIndicatorOrder();

    // Sauvegarder l'état de tri
    settings.setValue("sortColumn", sortColumn);
    settings.setValue("sortOrder", sortOrder);
}

void YaplukaWindow::on_categorie_widget_cellActivated(int row, int column)
{

}

