#ifndef SCHEDULEMANAGER_H
#define SCHEDULEMANAGER_H

class ScheduleManager
{
public:
    ScheduleManager();

    void addTask();

    void deleteTask(int id);

    void editTask(int id);

    void showTasks();
};

#endif