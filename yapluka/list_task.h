#ifndef LIST_TASK_H
#define LIST_TASK_H

#include "task.h"
#include "list_category.h"


class list_task{
public:
    list_task(){
    };

    void add_task( task* t)
    {
        master_->add_sub_task(t);
        qDebug()<<"list_task add_task id = "<< t->subject_;
    }

    void delete_task(task* t)
    {
        master_->delete_task(t);
    }

    task* get_task( QString id);

    void init(QString FileName,
              list_category& categories_);

    void save( QDomDocument& document, QDomElement& elroot);

    void update_display(QTreeWidget* task_widget,
                        bool cache = true);
private:
    task* master_ = nullptr;


//     QString subject;
//     unsigned int priority;
//     QString id;
//     unsigned int percentageComplete;
//     category* category_ = nullptr;
};

#endif
