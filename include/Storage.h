#ifndef STORAGE_H
#define STORAGE_H

#include <QList>
#include <QMap>
#include <QString>
#include "Task.h"

class Storage
{
public:
    // 任务操作：增加 username 参数
    static bool saveTasks(const QList<Task>& tasks, const QString& username, const QString& filename = "");
    static QList<Task> loadTasks(const QString& username, const QString& filename = "");

    // 用户操作：不变
    static bool saveUsers(const QMap<QString, QString>& users, const QString& filename = "data/users.json");
    static QMap<QString, QString> loadUsers(const QString& filename = "data/users.json");

private:
    static void ensureDirectoryExists();
    static QString getTaskFilePath(const QString& username, const QString& filename = "");
};

#endif
