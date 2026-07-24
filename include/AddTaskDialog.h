#ifndef ADD_TASK_DIALOG_H
#define ADD_TASK_DIALOG_H

#include <QDialog>
#include "Task.h"

namespace Ui { class AddTaskDialog; }

class AddTaskDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddTaskDialog(QWidget *parent = nullptr);
    ~AddTaskDialog();

    Task getTask() const;
    void setTask(const Task &t); // 用于修改

private:
    Ui::AddTaskDialog *ui;
};

#endif // ADD_TASK_DIALOG_H
