#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AddTaskDialog.h"
#include "Storage.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QDateTime>

MainWindow::MainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_username(username), m_nextId(1), m_remindThread(nullptr), m_reminder(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("日程管理 - " + username);

    connect(ui->pushButton_Add, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(ui->pushButton_Delete, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(ui->pushButton_Edit, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(ui->pushButton_Refresh, &QPushButton::clicked, this, &MainWindow::onRefreshList);
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &MainWindow::onRefreshList);
    connect(ui->pushButton_Complete, &QPushButton::clicked, this, &MainWindow::onToggleComplete);

    loadTasks();
    onDateSelected(QDate::currentDate());
}

MainWindow::~MainWindow()
{
    if(m_remindThread) {
        m_remindThread->quit();
        m_remindThread->wait();
    }
    delete ui;
}

void MainWindow::loadTasks()
{
    m_tasks = Storage::loadTasks();
    for(const Task &t : m_tasks) {
        if(t.id >= m_nextId) m_nextId = t.id + 1;
    }
}

void MainWindow::saveTasks()
{
    Storage::saveTasks(m_tasks);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveTasks();
    event->accept();
}

void MainWindow::onAddTask()
{
    AddTaskDialog dlg(this);
    if(dlg.exec() == QDialog::Accepted) {
        Task newTask = dlg.getTask();
        for(const Task &t : m_tasks) {
            if(t.isSameIdentity(newTask)) {
                QMessageBox::warning(this, "错误", "任务名称+开始时间已存在，请修改");
                return;
            }
        }
        newTask.id = m_nextId++;
        m_tasks.append(newTask);
        saveTasks();
        onRefreshList();
    }
}

void MainWindow::onDeleteTask()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "提示", "请先选中要删除的任务");
        return;
    }
    int id = ui->tableWidget->item(row, 0)->text().toInt();
    if(QMessageBox::question(this, "确认", "确定删除该任务吗？") == QMessageBox::Yes) {
        for(int i = 0; i < m_tasks.size(); ++i) {
            if(m_tasks[i].id == id) {
                m_tasks.removeAt(i);
                break;
            }
        }
        saveTasks();
        onRefreshList();
    }
}

void MainWindow::onEditTask()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "提示", "请先选中要修改的任务");
        return;
    }
    int id = ui->tableWidget->item(row, 0)->text().toInt();
    Task *target = nullptr;
    for(Task &t : m_tasks) {
        if(t.id == id) { target = &t; break; }
    }
    if(!target) return;

    AddTaskDialog dlg(this);
    dlg.setTask(*target);
    if(dlg.exec() == QDialog::Accepted) {
        Task newData = dlg.getTask();
        for(const Task &t : m_tasks) {
            if(t.id != id && t.isSameIdentity(newData)) {
                QMessageBox::warning(this, "错误", "任务名称+开始时间已存在");
                return;
            }
        }
        target->name = newData.name;
        target->startTime = newData.startTime;
        target->priority = newData.priority;
        target->category = newData.category;
        target->remindTime = newData.remindTime;
        saveTasks();
        onRefreshList();
    }
}

void MainWindow::onToggleComplete()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0) return;
    int id = ui->tableWidget->item(row, 0)->text().toInt();
    for(Task &t : m_tasks) {
        if(t.id == id) {
            t.isCompleted = !t.isCompleted;
            break;
        }
    }
    saveTasks();
    onRefreshList();
}

void MainWindow::onRefreshList()
{
    QDate date = ui->calendarWidget->selectedDate();
    onDateSelected(date);
}

void MainWindow::onDateSelected(const QDate &date)
{
    QList<Task> filtered;
    for(const Task &t : m_tasks) {
        if(t.startTime.date() == date) {
            filtered.append(t);
        }
    }
    std::sort(filtered.begin(), filtered.end(), [](const Task &a, const Task &b) {
        return a.startTime < b.startTime;
    });
    refreshTable(filtered);
}

void MainWindow::refreshTable(const QList<Task> &tasks)
{
    ui->tableWidget->setRowCount(tasks.size());
    ui->tableWidget->setColumnCount(7);
    QStringList headers = {"ID", "名称", "开始时间", "优先级", "分类", "提醒时间", "状态"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for(int i = 0; i < tasks.size(); ++i) {
        const Task &t = tasks[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(t.name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(t.startTime.toString("yyyy-MM-dd HH:mm")));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(Task::priorityToString(t.priority)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(Task::categoryToString(t.category)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(t.remindTime.toString("yyyy-MM-dd HH:mm")));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(t.isCompleted ? "已完成" : "未完成"));
    }
    ui->tableWidget->resizeColumnsToContents();
}