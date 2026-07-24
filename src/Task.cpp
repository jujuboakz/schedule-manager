#include "Task.h"
#include <iostream>
#include <iomanip>
#include <QDebug>

Task::Task() : 
    id(0), 
    priority(Priority::MEDIUM), 
    category(Category::LIFE), 
    completed(false) {}

Task::Task(
        int id,
        const QString& name,
        const QDateTime& startTime,
        const QDateTime& remindTime,
        Priority priority,
        Category category,
        bool completed
):
    id(id),
    name(name),
    startTime(startTime),
    remindTime(remindTime),
    priority(priority),
    category(category),
    completed(completed){};


int Task::getId() const{
    return id;
};

QString Task::getName() const{
    return name;
};

QDateTime Task::getStartTime() const{
    return startTime;
};

void Task::complete(){
    completed = true;
}

bool Task::isFinished(){
    return completed;
}

void Task::print() const
{
    //后续适配Qt输出
}

QString Task::priorityToString() const
{
    switch(priority)
    {
        case Priority::HIGH:
            return "高";

        case Priority::MEDIUM:
            return "中";

        case Priority::LOW:
            return "低";
    }

    return "中";
}

QString Task::categoryToString() const
{
    switch(category)
    {
        case Category::STUDY:
            return "学习";

        case Category::ENTERTAINMENT:
            return "娱乐";

        case Category::LIFE:
            return "生活";
    }

    return "生活";
}

QJsonObject Task::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["startTime"] = startTime.toString(Qt::ISODate);
    obj["priority"] = static_cast<int>(priority);
    obj["category"] = static_cast<int>(category);
    obj["remindTime"] = remindTime.toString(Qt::ISODate);
    obj["isCompleted"] = completed;
    return obj;
}

Task Task::fromJson(const QJsonObject &obj) {
    Task t;
    t.id = obj["id"].toInt();
    t.name = obj["name"].toString();
    t.startTime = QDateTime::fromString(obj["startTime"].toString(), Qt::ISODate);
    t.priority = static_cast<Priority>(obj["priority"].toInt());
    t.category = static_cast<Category>(obj["category"].toInt());
    t.remindTime = QDateTime::fromString(obj["remindTime"].toString(), Qt::ISODate);
    t.completed = obj["isCompleted"].toBool();
    return t;
}

bool Task::isSameIdentity(const Task &other) const {
    return (this->name == other.name && this->startTime == other.startTime);
}