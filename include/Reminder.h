#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include "Task.h"

// ========== 前置声明，避免循环依赖 ==========
class MainWindow;

class Reminder : public QObject
{
    Q_OBJECT
public:
    // ========== 修改：不再接收 tasks 指针，改为接收 MainWindow 指针 ==========
    explicit Reminder(MainWindow *mainWindow, QObject *parent = nullptr);
    void start(int intervalSec = 30);

signals:
    void remindSignal(const Task &task);

private slots:
    void checkReminders();

private:
    // ========== 修改：不再持有 m_tasks 指针 ==========
    MainWindow *m_mainWindow;
    QTimer *m_timer;
    QList<int> m_remindedIds;
};

#endif
