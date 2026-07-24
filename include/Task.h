#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

enum class Priority
{
    HIGH,
    MEDIUM,
    LOW
};


enum class Category
{
    STUDY,
    ENTERTAINMENT,
    LIFE
};



class Task
{

public:

    Task();

    Task(
        int id,
        const QString& name,
        const QDateTime& startTime,
        const QDateTime& remindTime,
        Priority priority = Priority::MEDIUM,
        Category category = Category::LIFE,
        bool completed = false
    );


    //getter
    int getId() const;

    QString getName() const;

    QDateTime getStartTime() const;

    //JSON序列化支持
    QJsonObject toJson() const;

    static Task fromJson(const QJsonObject &obj);

    bool isSameIdentity(const Task &other) const;

    //打印任务
    void print() const;

    //辅助函数
    QString priorityToString() const;

    QString categoryToString() const;

    void complete();

    bool isFinished();


private:

    int id;

    QString name;

    QDateTime startTime;

    Priority priority;

    Category category;

    QDateTime remindTime;

    bool completed;

};


#endif