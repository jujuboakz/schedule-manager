#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include "Task.h"

class Reminder : public QObject
{
    Q_OBJECT
public:
    explicit Reminder(const QList<Task> *tasks, QObject *parent = nullptr);
    void start(int intervalSec = 30);

signals:
    void remindSignal(const Task &task);

private slots:
    void checkReminders();

private:
    const QList<Task> *m_tasks;
    QTimer *m_timer;
    QList<int> m_remindedIds;
};

#endif