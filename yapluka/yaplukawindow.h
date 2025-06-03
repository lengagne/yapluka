#ifndef YAPLUKAWINDOW_H
#define YAPLUKAWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>

#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QTreeWidgetItem>

#include "list_category.h"
#include "list_task.h"

QT_BEGIN_NAMESPACE
namespace Ui { class YaplukaWindow; }
QT_END_NAMESPACE



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

    void on_cachefinibox_stateChanged(int arg1);

    void save();

    void showContextMenu(const QPoint &pos);

//    void showTaskDetails(QTreeWidgetItem* item, int column);

    void editTask(QTreeWidgetItem* item, int column);

    void updateTask( );

    void on_actionnouvelle_tache_triggered();

    void on_action_finish_tache_triggered();

    void onActionDeleteTask();

    void onActionEdit();
    void on_actionEnresitrer_sous_triggered();

    void on_BoutonNouvelleTache_clicked();

    void on_BoutonEditTache_clicked();

    void on_BoutonFinirTache_clicked();

    void on_BoutonSupprimerTache_clicked();

private:
    void contextMenuEvent(QContextMenuEvent *event) override ;

    void loadSettings();

    void read_file();

    void saveSettings() const;

    void update_list();

    Ui::YaplukaWindow *ui;

    list_task tasks_;

    list_category categories_;

    QString currentFileName_;
    QList<QAction*> columnActions;
    QSize window_size_;

    bool cache_fini_ = true;

};
#endif // YAPLUKAWINDOW_H
