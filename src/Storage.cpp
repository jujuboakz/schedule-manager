#include "Storage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <unistd.h>
#include <linux/limits.h>

static QString getExecutableDir() {
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        QString exePath = QString::fromLocal8Bit(buf);
        QFileInfo info(exePath);
        return info.absolutePath();
    }
    return QDir::currentPath();
}

static QString getDataDir() {
    return getExecutableDir() + "/data";
}

// ========== 新增：去掉路径中多余的 data/ 前缀 ==========
static QString normalizeDataPath(const QString &path) {
    if (path.startsWith("data/")) {
        return path.mid(5);
    }
    return path;
}

void Storage::ensureDirectoryExists() {
    QString dataPath = getDataDir();
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
        qDebug() << "创建 data 目录:" << dataPath;
    }
}

QString Storage::getTaskFilePath(const QString& username, const QString& filename) {
    if (!filename.isEmpty()) {
        return getDataDir() + "/" + normalizeDataPath(filename);
    }
    return getDataDir() + "/tasks_" + username + ".json";
}

bool Storage::saveTasks(const QList<Task>& tasks, const QString& username, const QString& filename) {
    ensureDirectoryExists();
    QString fullPath = getTaskFilePath(username, filename);
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "saveTasks 打开文件失败:" << file.errorString();
        qDebug() << "尝试写入路径:" << fullPath;
        return false;
    }

    QJsonArray arr;
    for (const Task &t : tasks) {
        arr.append(t.toJson());
    }
    QJsonDocument doc(arr);
    file.write(doc.toJson());
    file.close();
    qDebug() << "saveTasks 成功写入:" << fullPath << ", 任务数:" << tasks.size();
    return true;
}

QList<Task> Storage::loadTasks(const QString& username, const QString& filename) {
    QList<Task> tasks;
    QString fullPath = getTaskFilePath(username, filename);
    QFile file(fullPath);
    if (!file.exists()) {
        qDebug() << "loadTasks: 文件不存在:" << fullPath << "，返回空列表";
        return tasks;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "loadTasks: 打开文件失败:" << fullPath;
        return tasks;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        qDebug() << "loadTasks: JSON 格式错误（不是数组）";
        return tasks;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        tasks.append(Task::fromJson(val.toObject()));
    }
    file.close();
    qDebug() << "loadTasks: 成功加载" << tasks.size() << "个任务，来自:" << fullPath;
    return tasks;
}

bool Storage::saveUsers(const QMap<QString, QString>& users, const QString& filename) {
    ensureDirectoryExists();
    QString baseName = normalizeDataPath(filename.isEmpty() ? "users.json" : filename);
    QString fullPath = getDataDir() + "/" + baseName;
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "saveUsers 打开文件失败:" << file.errorString();
        qDebug() << "尝试写入路径:" << fullPath;
        return false;
    }

    QJsonObject obj;
    for (auto it = users.begin(); it != users.end(); ++it) {
        obj[it.key()] = it.value();
    }
    QJsonDocument doc(obj);
    file.write(doc.toJson());
    file.close();
    qDebug() << "saveUsers 成功写入:" << fullPath << ", 用户数:" << users.size();
    return true;
}

QMap<QString, QString> Storage::loadUsers(const QString& filename) {
    QMap<QString, QString> users;
    QString baseName = normalizeDataPath(filename.isEmpty() ? "users.json" : filename);
    QString fullPath = getDataDir() + "/" + baseName;
    QFile file(fullPath);
    if (!file.exists()) {
        qDebug() << "loadUsers: 文件不存在:" << fullPath;
        return users;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "loadUsers: 打开文件失败:" << fullPath;
        return users;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        qDebug() << "loadUsers: JSON 格式错误（不是对象）";
        return users;
    }

    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        users[it.key()] = it.value().toString();
    }
    file.close();
    qDebug() << "loadUsers: 成功加载" << users.size() << "个用户，来自:" << fullPath;
    return users;
}
