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

task_dialog::task_dialog(list_category* lcat,
                         task* t, QWidget *parent)
    : QDialog(parent), currentTask(t), lcat_(lcat) {
    initUI();
    loadTaskData();
}

void task_dialog::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    subjectEdit = new QLineEdit(this);
    prioritySpinBox = new QSpinBox(this);
    idEdit = new QLabel(this);
    cat_of = new QComboBox(this);
    QList<QString> cats;
    lcat_->get_categories(cats);
    int cpt = 1;
    for (const QString &item : cats) {
        cat_of->addItem(item, cpt++);
    }

    // il faut supprimer de l'ancienne categorie et ajouter a la nouvelle

//    statusComboBox = new QComboBox(this);
    percentageSpinBox = new QSpinBox(this);
    percentageSpinBox->setMaximum(100);
    descriptionEdit = new QTextEdit(this);
    actualStartDateEdit = new QDateTimeEdit(this);
    creationDateEdit = new QDateTimeEdit(this);
    completionDateEdit = new QDateTimeEdit(this);
    modificationDateEdit = new QDateTimeEdit(this);

    // Désactiver les champs de date pour qu'ils ne soient pas modifiables
    actualStartDateEdit->setEnabled(false);
    creationDateEdit->setEnabled(false);
    completionDateEdit->setEnabled(false);
    modificationDateEdit->setEnabled(false);

    formLayout->addRow("Subject:", subjectEdit);
    formLayout->addRow("Priority:", prioritySpinBox);
    formLayout->addRow("ID:", idEdit);
    formLayout->addRow("Catégorie:", cat_of);
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

