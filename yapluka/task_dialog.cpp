/*
#include "task_dialog.h"
#include <QTextEdit>


task_dialog::task_dialog(QTreeWidgetItem* taskItem,
                         task* task,
                         QWidget* parent) : QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* idQ = new QLabel("id:" + task->id_, this);
    layout->addWidget(idQ);

    QHBoxLayout * titreLayout = new QHBoxLayout;
    QLabel* titreQ = new QLabel("titre : ", this);
    QLineEdit* titreEdit = new QLineEdit(task->subject_, this);
    editFields_.append(titreEdit);
    titreLayout->addWidget(titreQ);
    titreLayout->addWidget(titreEdit);
    layout->addLayout(titreLayout);

    QHBoxLayout * priority_Layout = new QHBoxLayout;
    QLabel* priorityQ = new QLabel("priorité : ", this);
    QLineEdit* priorityEdit = new QLineEdit(QString::number(task->priority_), this);
    editFields_.append(priorityEdit);
    priority_Layout->addWidget(priorityQ);
    priority_Layout->addWidget(priorityEdit);
    layout->addLayout(priority_Layout);

    QHBoxLayout * pourcent_Layout = new QHBoxLayout;
    QLabel* pourcentQ = new QLabel("pourcentage : ", this);
    QLineEdit* pourcentEdit = new QLineEdit(QString::number(task->percentage_), this);

    pourcent_Layout->addWidget(pourcentQ);

    // Créer un QSlider pour le pourcentage
    QSlider* pourcentSlider = new QSlider(Qt::Horizontal, this);
    pourcentSlider->setMinimum(0);
    pourcentSlider->setMaximum(100);
    pourcentSlider->setValue(task->percentage_);

    pourcent_Layout->addWidget(pourcentSlider);
    pourcent_Layout->addWidget(pourcentEdit);

    layout->addLayout(pourcent_Layout);


    // Connecter le signal valueChanged du QSlider à une fonction qui met à jour le pourcentage
    connect(pourcentSlider, &QSlider::valueChanged, [=](int value) {
        task->percentage_ = value;
        pourcentEdit->setText(QString::number(task->percentage_));
        // Vous pouvez également mettre à jour l'affichage ou d'autres éléments ici si nécessaire
    });
    editFields_.append(pourcentEdit);

    QLabel* descriptionQ = new QLabel("description : ", this);
    QTextEdit* descriptionEdit = new QTextEdit(task->description_, this);

    layout->addWidget(descriptionQ);
    layout->addWidget(descriptionEdit);
    editFields_.append(descriptionEdit);

    // Ajouter un bouton pour sauvegarder les modifications
    QPushButton* saveButton = new QPushButton("Save", this);
    layout->addWidget(saveButton);

    setLayout(layout);
    setWindowTitle("Edit Task");

    connect(saveButton, &QPushButton::clicked, this, &task_dialog::saveChanges);
}

void task_dialog::saveChanges()
{
    // Émettre un signal avec les nouvelles données
    emit taskEdited(editFields_);
    accept();
}

*/
#include "task_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>

task_dialog::task_dialog(QWidget *parent)
    : QDialog(parent), currentTask(new task()) {
    initUI();
}

task_dialog::task_dialog(task* t, QWidget *parent)
    : QDialog(parent), currentTask(t) {
    initUI();
    loadTaskData();
}

void task_dialog::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    subjectEdit = new QLineEdit(this);
    prioritySpinBox = new QSpinBox(this);
    idEdit = new QLabel(this);
//    statusComboBox = new QComboBox(this);
    percentageSpinBox = new QSpinBox(this);
    descriptionEdit = new QTextEdit(this);
    actualStartDateEdit = new QDateTimeEdit(this);
    creationDateEdit = new QDateTimeEdit(this);
    completionDateEdit = new QDateTimeEdit(this);
    modificationDateEdit = new QDateTimeEdit(this);

    formLayout->addRow("Subject:", subjectEdit);
    formLayout->addRow("Priority:", prioritySpinBox);
    formLayout->addRow("ID:", idEdit);
//    formLayout->addRow("Status:", statusComboBox);
    formLayout->addRow("Percentage:", percentageSpinBox);
    formLayout->addRow("Description:", descriptionEdit);
    formLayout->addRow("Actual Start Date:", actualStartDateEdit);
    formLayout->addRow("Creation Date:", creationDateEdit);
    formLayout->addRow("Completion Date:", completionDateEdit);
    formLayout->addRow("Modification Date:", modificationDateEdit);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    connect(okButton, &QPushButton::clicked, this, &task_dialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle("Task Dialog");
}

void task_dialog::loadTaskData() {
    if (currentTask) {
        subjectEdit->setText(currentTask->subject_);
        prioritySpinBox->setValue(currentTask->priority_);
        idEdit->setText(currentTask->id_);
//        statusComboBox->setCurrentIndex(currentTask->status_);
        percentageSpinBox->setValue(currentTask->percentage_);
        descriptionEdit->setText(currentTask->description_);
        actualStartDateEdit->setDateTime(currentTask->actualstartdate_);
        creationDateEdit->setDateTime(currentTask->creationdate_);
        completionDateEdit->setDateTime(currentTask->completiondate_);
        modificationDateEdit->setDateTime(currentTask->modificationdate_);
    }
}

void task_dialog::accept() {
    if (currentTask) {
        currentTask->subject_ = subjectEdit->text();
        currentTask->priority_ = prioritySpinBox->value();
        //currentTask->status_ = statusComboBox->currentIndex();
        currentTask->percentage_ = percentageSpinBox->value();
        currentTask->description_ = descriptionEdit->toPlainText();
        currentTask->actualstartdate_ = actualStartDateEdit->dateTime();
        currentTask->creationdate_ = creationDateEdit->dateTime();
        currentTask->completiondate_ = completionDateEdit->dateTime();
        currentTask->modificationdate_ = modificationDateEdit->dateTime();        

        qDebug()<<"on sauvegarde la tache : "<< currentTask->subject_;
        qDebug()<<"id : "<< currentTask->id_;
    }
    QDialog::accept();
}

/*task* task_dialog::getTask() const {
    return currentTask;
}*/
