#ifndef YAPLUKAWINDOW_H
#define YAPLUKAWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
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

    void showContextMenu(const QPoint &pos);

    void on_cachefinibox_stateChanged(int arg1);

private:
    void loadSettings();

    void read_file();

    void saveSettings() const;

    void update_list();

    Ui::YaplukaWindow *ui;

    list_task tasks_;

    list_category categories_;

    QString currentFileName_;
    QList<QAction*> columnActions;

    bool cache_fini_ = true;

};
#endif // YAPLUKAWINDOW_H
