#include "Reminder.h"
#include <QDateTime>

Reminder::Reminder(const QList<Task> *tasks, QObject *parent)
    : QObject(parent), m_tasks(tasks)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Reminder::checkReminders);
}

void Reminder::start(int intervalSec) {
    m_timer->start(intervalSec * 1000);
}

void Reminder::checkReminders() {
    if (!m_tasks) return;

    QDateTime now = QDateTime::currentDateTime();

    for (const Task &task : *m_tasks) {
        if (!task.isFinished() && task.remindTime <= now && !m_remindedIds.contains(task.id)) {
            m_remindedIds.append(task.id);
            emit remindSignal(task);
        }
    }
}