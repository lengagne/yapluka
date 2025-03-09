#include "yaplukawindow.h"
#include "./ui_yaplukawindow.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QSettings>


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
    QVBoxLayout * verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(ui->tableWidget);

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
    tasks_ = reader.lireFichier(currentFileName_);

    update_list();
}

void YaplukaWindow::update_list()
{
    ui->tableWidget->clear();

    QStringList headers;
    nb_colonnes_=0;
    for (int i=0;i<5;i++)
    {
        qDebug()<<"print_option_["<< i<<"] = "<< print_option_[i];
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
                          item = new QTableWidgetItem();
                          item->setData(Qt::DisplayRole, p.percentageComplete);
                          item->setData(Qt::EditRole, p.percentageComplete);
                          break;
                      }
                      default: qDebug() << "This should not happen";
                  }

                  // Exemple : Colorer les cellules en fonction de la priorité
                  if ( p.priority > 5) {
                      item->setBackground(QBrush(QColor(255, 100, 100))); // Rouge clair pour haute priorité
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



QList<Task> TaskReader::lireFichier(const QString &filePath)
{
    QList<Task> tasks;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Erreur d'ouverture du fichier");
        return tasks;
    }

    QXmlStreamReader xmlReader(&file);

    Task currentTask;

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
        }
        // Si l'élément est une balise de fin
        else if (xmlReader.isEndElement() && xmlReader.name() == "task") {
            tasks.append(currentTask);
        }
    }

    if (xmlReader.hasError()) {
        qWarning("Erreur de parsing XML");
    }

    file.close();
    return tasks;
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


    ui->actionid->setChecked(print_option_[0]);
    update_list();
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
}
