#include "Reminder.h"
#include "MainWindow.h" 
#include <QDateTime>
#include <QDebug>
#include <QThread>

// ========== 修改：构造函数接收 MainWindow 指针 ==========
Reminder::Reminder(MainWindow *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow)
{
    qDebug() << "=== Reminder 构造函数被调用 ===";
    qDebug() << "接收到的 MainWindow 指针:" << m_mainWindow;

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

    // ========== 检查 MainWindow 指针是否有效 ==========
    if (!m_mainWindow) {
        qDebug() << "错误：m_mainWindow 指针为空！";
        return;
    }

    // ========== 通过 MainWindow 的加锁接口获取任务列表拷贝 ==========
    QList<Task> tasks = m_mainWindow->getTasksCopy();

    qDebug() << "当前任务数量:" << tasks.size();
    if (tasks.isEmpty()) {
        qDebug() << "提示：任务列表为空，没有任务可检查";
        return;
    }

    // ========== 检查每个任务的条件 ==========
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "当前时间:" << now.toString("yyyy-MM-dd hh:mm:ss");

    for(const Task &task : tasks) {
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
