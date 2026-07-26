#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QThread>
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

    // 多线程提醒
    QThread *m_remindThread;
    Reminder *m_remindWorker;

    // 语音识别
    VoiceRecognizer *m_voiceRecognizer;
    QString m_voiceTaskName;
};

#endif // MAINWINDOW_H
