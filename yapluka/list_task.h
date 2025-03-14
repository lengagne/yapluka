#ifndef LIST_TASK_H
#define LIST_TASK_H

#include "task.h"
#include "list_category.h"


class list_task{
public:
    list_task(){ };

    void init(QString FileName,
              list_category& categories_);

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
