#ifndef STORAGE_H
#define STORAGE_H

#include <QList>
#include <QMap>
#include <QString>
#include <QDateTime>
#include "Task.h"

class Storage
{
public:
    //任务数据操作
    static bool saveTasks(const QList<Task>&, const QString &filename = "data/tasks.json");

    static QList<Task> loadTasks(const QString &filename = "data/tasks.json");
    
    //用户数据操作
    static bool saveUsers(const QMap<QString,QString>&users, const QString &filename = "data/users.json");
    
    static QMap<QString,QString> loadUsers(const QString &filename = "data/users.json");

private:
    static void ensureDirectoryExists();
};

#endif