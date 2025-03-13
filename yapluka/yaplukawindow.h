#ifndef YAPLUKAWINDOW_H
#define YAPLUKAWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QFile>
#include <QList>
#include <QString>
#include <QDebug>
#include <QListWidgetItem>
#include "category.h"

QT_BEGIN_NAMESPACE
namespace Ui { class YaplukaWindow; }
QT_END_NAMESPACE


struct Task {
    QString subject;
    unsigned int priority;
    QString id;
    unsigned int percentageComplete;
};

struct AffichOption{
    bool affich_fini_ = false;
};

class TaskReader {
public:
    void lireFichier(const QString &filePath,
                           QList<Task>& tasks/*,
                           QList<category>& categories*/);
};



class YaplukaWindow : public QMainWindow
{
    Q_OBJECT

public:
    YaplukaWindow(QWidget *parent = nullptr);
    ~YaplukaWindow();

private slots:
    void on_actionOuvrir_triggered();

    void on_actionEnregistrer_triggered();

    void on_actionQuitter_triggered();

    void on_treeView_collapsed(const QModelIndex &index);

    // void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void resizeEvent(QResizeEvent *event);

    void on_actionMontrer_fini_triggered();

    void on_actionCacher_fini_triggered();

    void on_actionid_triggered();

    void on_actiontache_2_triggered();

    void on_actioncategorie_triggered();

    void on_actionpriorite_triggered();

    void on_actionpourcentage_triggered();

    void on_categorie_widget_cellActivated(int row, int column);

private:
    void loadSettings();

    void read_file();

    void saveSettings() const;

    void update_list();

    Ui::YaplukaWindow *ui;

    AffichOption affich_option_;

    bool print_option_[5];

    QList<Task> tasks_;

    list_category categories_;

    unsigned int nb_colonnes_;

    QString currentFileName_;

};
#endif // YAPLUKAWINDOW_H
