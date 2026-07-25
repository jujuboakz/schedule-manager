#include "Reminder.h"
#include <QDateTime>
#include <QDebug>
#include <QThread> 

Reminder::Reminder(const QList<Task> *tasks, QObject *parent)
    : QObject(parent), m_tasks(tasks)
{
    qDebug() << "=== Reminder 构造函数被调用 ===";
    qDebug() << "接收到的任务列表指针:" << m_tasks;
    qDebug() << "当前任务数量:" << (m_tasks ? m_tasks->size() : 0);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Reminder::checkReminders);
}

void Reminder::start(int intervalSec) {
    qDebug() << "=== Reminder::start() 被调用，间隔:" << intervalSec << "秒 ===";
    m_timer->start(intervalSec * 1000);
}

void Reminder::checkReminders() {
    // ========== 调试信息1：函数被调用 ==========
    qDebug() << "========================================";
    qDebug() << ">>> checkReminders() 被执行";
    qDebug() << "当前线程ID:" << QThread::currentThreadId();

    // ========== 调试信息2：检查任务列表是否有效 ==========
    if (!m_tasks) {
        qDebug() << "错误：m_tasks 指针为空！";
        return;
    }

    qDebug() << "当前任务数量:" << m_tasks->size();
    if (m_tasks->isEmpty()) {
        qDebug() << "提示：任务列表为空，没有任务可检查";
        return;
    }

    // ========== 调试信息3：检查每个任务的条件 ==========
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "当前时间:" << now.toString("yyyy-MM-dd hh:mm:ss");

    for(const Task &task : *m_tasks) {
        // 获取条件变量
        bool timeCondition = task.remindTime <= now;
        
        bool completedCondition = task.isFinished(); 
        
        bool remindedCondition = m_remindedIds.contains(task.id);

        qDebug() << "  ----- 任务 ID:" << task.id << " -----";
        qDebug() << "  名称:" << task.name;
        qDebug() << "  开始时间:" << task.startTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "  提醒时间:" << task.remindTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "  已完成:" << completedCondition;
        qDebug() << "  已提醒过:" << remindedCondition;
        qDebug() << "  时间条件(提醒时间<=当前):" << timeCondition;
        qDebug() << "  触发条件 (未完成 && 时间满足 && 未提醒过):"
                 << (!completedCondition && timeCondition && !remindedCondition);

        // ========== 实际提醒逻辑 ==========
        // 同样，这里使用了 task.isFinished()
        if(!task.isFinished() && task.remindTime <= now && !m_remindedIds.contains(task.id)) {
            qDebug() << "  >>>>>> 条件满足！发送提醒信号！任务:" << task.name;
            m_remindedIds.append(task.id);
            emit remindSignal(task);
        } else {
            qDebug() << "  条件不满足，跳过";
        }
    }

    // ========== 调试信息4：已提醒ID列表 ==========
    qDebug() << "已提醒ID列表:" << m_remindedIds;
    qDebug() << "========================================";
}