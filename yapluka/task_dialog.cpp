#include "task_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QMessageBox>


task_dialog::task_dialog(QWidget *parent)
    : QDialog(parent), currentTask(new task()) {
    initUI();
}

// task_dialog::task_dialog(list_category* lcat,
//                          task* t, QWidget *parent)
//     : QDialog(parent), currentTask(t), lcat_(lcat) {
//     initUI();
//     loadTaskData();
// }

task_dialog::task_dialog(list_category* lcat,
                         task* t,
                         QWidget* parent,
                         bool creation)
    : QDialog(parent),
      currentTask(t),
      lcat_(lcat),
      creation_(creation)
{
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
    cat_of->addItem(
        tr("(Sans catégorie)"),
        QVariant::fromValue(static_cast<category*>(nullptr))
    );

    // Même ordre et mêmes chemins que dans l'éditeur de catégorie.
    for (category *cat : lcat_->toutes_categories()) {
        QString chemin = cat->name_;

        for (category *parent = lcat_->parent_de(cat);
            parent;
            parent = lcat_->parent_de(parent)) {
            chemin.prepend(parent->name_ + " / ");
        }

        cat_of->addItem(chemin, QVariant::fromValue(cat));
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
    formLayout->addRow("Percentage:", percentageSpinBox);
    formLayout->addRow("Description:", descriptionEdit);
    formLayout->addRow("Actual Start Date:", actualStartDateEdit);
    formLayout->addRow("Creation Date:", creationDateEdit);
    formLayout->addRow("Completion Date:", completionDateEdit);
    formLayout->addRow("Modification Date:", modificationDateEdit);

    deadlineMode = new QComboBox(this);
    deadlineMode->addItem(tr("Choisir…"));                  // 0
    deadlineMode->addItem(tr("Avec une date limite"));      // 1
    deadlineMode->addItem(tr("Pas de deadline (-1)"));       // 2

    deadlineEdit = new QDateEdit(QDate::currentDate(), this);
    deadlineEdit->setCalendarPopup(true);
    deadlineEdit->setDisplayFormat("dd/MM/yyyy");
    deadlineEdit->setEnabled(false);

    formLayout->addRow(tr("Deadline :"), deadlineMode);
    formLayout->addRow(tr("Date limite :"), deadlineEdit);

    connect(
        deadlineMode,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [this](int index) {
            deadlineEdit->setEnabled(index == 1);
        }
    );

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
        qDebug()<<"loadTaskData";
        subjectEdit->setText(currentTask->subject_);
        prioritySpinBox->setValue(currentTask->priority_);
        idEdit->setText(currentTask->id_);
        percentageSpinBox->setValue(currentTask->percentage_);
        descriptionEdit->setText(currentTask->description_);
        actualStartDateEdit->setDateTime(currentTask->actualstartdate_);
        creationDateEdit->setDateTime(currentTask->creationdate_);
        completionDateEdit->setDateTime(currentTask->completiondate_);
        modificationDateEdit->setDateTime(currentTask->modificationdate_);
        // if(currentTask->cat_)
        // {
        //     int index = cats_.indexOf(currentTask->cat_->name_);
        //     if (index !=-1)
        //     {
        //         cat_of->setCurrentIndex( index );
        //     }
        // }
        cat_of->setCurrentIndex(0);

        for (int i = 1; i < cat_of->count(); ++i) {
            category *cat = cat_of->itemData(i).value<category*>();

            if (cat == currentTask->cat_) {
                cat_of->setCurrentIndex(i);
                break;
            }
        }

        if (currentTask->deadline_.isValid())
            deadlineEdit->setDate(currentTask->deadline_);

        deadlineMode->setCurrentIndex(
            creation_
                ? 0
                : (currentTask->deadline_.isValid() ? 1 : 2)
        );

    }
}

void task_dialog::accept() {
    if (deadlineMode->currentIndex() == 0) {
        QMessageBox::warning(
            this,
            tr("Deadline"),
            tr("Choisissez une date limite ou « Pas de deadline ».")
        );
        return;
    }

    const unsigned int ancienPourcentage = currentTask->percentage_;

    if (currentTask) {
        currentTask->subject_ = subjectEdit->text();
        currentTask->priority_ = prioritySpinBox->value();
        //currentTask->status_ = statusComboBox->currentIndex();
        currentTask->percentage_ = percentageSpinBox->value();
        currentTask->description_ = descriptionEdit->toPlainText();
        // currentTask->actualstartdate_ = actualStartDateEdit->dateTime();
        // currentTask->creationdate_ = creationDateEdit->dateTime();
        // currentTask->completiondate_ = completionDateEdit->dateTime();
        // currentTask->modificationdate_ = modificationDateEdit->dateTime();

        // // il faut mettre à jour la liste des categories
        // category* new_cat = lcat_->get_cat_by_name(cat_of->currentText());
        // if (currentTask->cat_ != new_cat)
        // {
        //     qDebug()<<"Changement de category";
        //     // on supprime de l'ancienne categorie
        //     if(currentTask->cat_)
        //         currentTask->cat_->remove_task_by_id(currentTask->id_);
        //     qDebug()<<"on a enlever de l'ancienne";
        //     // on rajoute dans la nouvelle
        //     new_cat->add_task_by_id(currentTask->id_);
        //     qDebug()<<"on rajoute à la nouvelle";
        //     currentTask->cat_ = new_cat;
        // }
        category *new_cat = cat_of->currentData().value<category*>();

        if (currentTask->cat_ != new_cat) {
            if (currentTask->cat_) {
                currentTask->cat_->remove_task_by_id(currentTask->id_);
            }

            if (new_cat) {
                new_cat->add_task_by_id(currentTask->id_);
            }

            currentTask->cat_ = new_cat;
        }

        currentTask->deadline_ = deadlineMode->currentIndex() == 1 ? deadlineEdit->date() : QDate();

        const QDateTime maintenant = QDateTime::currentDateTime();

        currentTask->modificationdate_ = maintenant;

        // Premier démarrage : conserver ensuite cette date.
        if (currentTask->percentage_ > 0
            && !currentTask->actualstartdate_.isValid()) {
            currentTask->actualstartdate_ = maintenant;
        }

        // Passage à l'état terminé.
        if (currentTask->percentage_ >= 100) {
            if (ancienPourcentage < 100
                || !currentTask->completiondate_.isValid()) {
                currentTask->completiondate_ = maintenant;
            }

            currentTask->status_ = 1;
        } else {
            // Une tâche rouverte n'a plus de date de complétion.
            currentTask->completiondate_ = QDateTime();
            currentTask->status_ = 0;
        }

    }
    QDialog::accept();
}


