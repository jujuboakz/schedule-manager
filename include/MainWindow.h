#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QThread>
#include "Task.h"

class Reminder;  //Ç°ÖÃÉùÃ÷

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddTask();
    void onDeleteTask();
    void onEditTask();
    void onRefreshList();
    void onDateSelected(const QDate &date);
    void onToggleComplete();

private:
    void loadTasks();
    void saveTasks();
    void refreshTable(const QList<Task> &tasks);

    Ui::MainWindow *ui;
    QString m_username;
    QList<Task> m_tasks;
    int m_nextId;

    QThread *m_remindThread;
    Reminder *m_reminder;
};

#endif