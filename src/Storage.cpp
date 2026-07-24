#include "Storage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>


void Storage::ensureDirectoryExists() {
    QDir dir("data");
    if(!dir.exists()) {
        dir.mkpath(".");
    }
}

bool Storage::saveTasks(const QList<Task>& tasks, const QString &filename) {
    ensureDirectoryExists();
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray arr;
    for(const Task &t : tasks) {
        arr.append(t.toJson());
    }
    QJsonDocument doc(arr);
    file.write(doc.toJson());
    file.close();
    return true;
}

QList<Task> Storage::loadTasks(const QString &filename) {
    QList<Task> tasks;
    QFile file(filename);
    if(!file.exists()) {
        return tasks;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        return tasks;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if(!doc.isArray()) {
        return tasks;
    }

    QJsonArray arr = doc.array();
    for(const QJsonValue &val : arr) {
        tasks.append(Task::fromJson(val.toObject()));
    }
    file.close();
    return tasks;
}

bool Storage::saveUsers(const QMap<QString, QString>& users, const QString &filename) {
    ensureDirectoryExists();
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject obj;
    for(auto it = users.begin(); it != users.end(); ++it) {
        obj[it.key()] = it.value();
    }
    QJsonDocument doc(obj);
    file.write(doc.toJson());
    file.close();
    return true;
}

QMap<QString, QString> Storage::loadUsers(const QString &filename) {
    QMap<QString, QString> users;
    QFile file(filename);
    if(!file.exists()) {
        return users;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        return users;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if(!doc.isObject()) {
        return users;
    }

    QJsonObject obj = doc.object();
    for(auto it = obj.begin(); it != obj.end(); ++it) {
        users[it.key()] = it.value().toString();
    }
    file.close();
    return users;
}