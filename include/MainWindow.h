#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QThread>
#include <QMutex>
#include "Task.h"
#include "Reminder.h"
#include "VoiceRecognizer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

    // ========== 新增：供 Reminder 安全获取任务列表（加锁返回拷贝） ==========
    QList<Task> getTasksCopy() const;

private slots:
    void onAddTask();
    void onDeleteTask();
    void onEditTask();
    void onToggleComplete();
    void onRefreshList();
    void onDateSelected(const QDate &date);
    void onTaskReminded(const Task &task);

    // 语音识别的槽函数
    void onVoiceInput();
    void onVoiceResult(const QString &text);
    void onVoiceStatus(const QString &status);

private:
    void loadTasks();
    void saveTasks();
    void refreshTable(const QList<Task> &tasks);
    
    // 设置后台线程
    void setupRemindWorker();
    void setupVoiceRecognizer();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString m_username;
    QList<Task> m_tasks;
    int m_nextId;

    // ========== 新增：互斥锁，保护 m_tasks 的并发访问 ==========
    mutable QMutex m_taskMutex;

    // 多线程提醒
    QThread *m_remindThread;
    Reminder *m_remindWorker;

    // 语音识别
    VoiceRecognizer *m_voiceRecognizer;
    QString m_voiceTaskName;
};

#endif // MAINWINDOW_H
