/*#ifndef TASK_DIALOG_H
#define TASK_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QPushButton>

#include "task.h"

class task_dialog: public QDialog
{
     Q_OBJECT
public:
    task_dialog(QTreeWidgetItem* taskItem,
                task* task,
                QWidget* parent = nullptr);

private slots:
    void saveChanges() ;

signals:
    void taskEdited(const QList<QLineEdit*>& editFields);

private:
    QList<QLineEdit*> editFields_;
};

#endif
*/

#ifndef TASK_DIALOG_H
#define TASK_DIALOG_H

#include <QDialog>
#include <QLabel>
#include "task.h" // Assurez-vous d'inclure le fichier d'en-tête de la classe task

class QLineEdit;
class QSpinBox;
class QComboBox;
class QTextEdit;
class QDateTimeEdit;

class task_dialog : public QDialog {
    Q_OBJECT

public:
    task_dialog(QWidget *parent = nullptr);
    task_dialog(task* t, QWidget *parent = nullptr);


private slots:
    void accept() override;

private:
    void initUI(); // Déclaration de la fonction initUI
    void loadTaskData();

    QLineEdit *subjectEdit;
    QSpinBox *prioritySpinBox;
    QLabel *idEdit;
    QComboBox *statusComboBox;
    QSpinBox *percentageSpinBox;
    QTextEdit *descriptionEdit;
    QDateTimeEdit *actualStartDateEdit;
    QDateTimeEdit *creationDateEdit;
    QDateTimeEdit *completionDateEdit;
    QDateTimeEdit *modificationDateEdit;

    task* currentTask;
};

#endif // TASK_DIALOG_H
