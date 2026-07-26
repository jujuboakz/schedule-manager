#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AddTaskDialog.h"
#include "Storage.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QDateTime>
#include <QFile>
#include <QHeaderView>
#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <cstdlib>

MainWindow::MainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_username(username), m_nextId(1)
{
    ui->setupUi(this);
    this->setWindowTitle("📅 日程管家 - " + username);

    // // ========== 统计标签和分组框标题改为黑色 ==========
    // ui->label_total->setStyleSheet("color: #000000;");
    // ui->label_today->setStyleSheet("color: #000000;");
    // ui->label_done->setStyleSheet("color: #000000;");
    // ui->groupBox_Stats->setStyleSheet("QGroupBox::title { color: #000000; }");
       
    // ===================================================

    // 连接界面信号
    connect(ui->pushButton_Add, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(ui->pushButton_Delete, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(ui->pushButton_Edit, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(ui->pushButton_Refresh, &QPushButton::clicked, this, &MainWindow::onRefreshList);
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &MainWindow::onRefreshList);
    connect(ui->pushButton_Complete, &QPushButton::clicked, this, &MainWindow::onToggleComplete);

    // 加载数据
    loadTasks();

    // 初始化界面：显示今日任务
    onDateSelected(QDate::currentDate());

    // 启动后台提醒线程
    setupRemindWorker();

    // 初始化语音识别
    setupVoiceRecognizer();
}

MainWindow::~MainWindow() {
    if(m_remindThread) {
        m_remindThread->quit();
        m_remindThread->wait();
    }
    delete ui;
}

void MainWindow::setupRemindWorker() {
    m_remindThread = new QThread(this);
    m_remindWorker = new Reminder(&m_tasks);
    m_remindWorker->moveToThread(m_remindThread);

    connect(m_remindThread, &QThread::started, [this]() {
        m_remindWorker->start(30);
    });
    connect(m_remindWorker, &Reminder::remindSignal, this, &MainWindow::onTaskReminded);
    connect(m_remindThread, &QThread::finished, m_remindWorker, &QObject::deleteLater);

    m_remindThread->start();
}

void MainWindow::loadTasks() {
    m_tasks = Storage::loadTasks(m_username);
    for(const Task &t : m_tasks) {
        if(t.id >= m_nextId) m_nextId = t.id + 1;
    }
}

void MainWindow::saveTasks() {
    Storage::saveTasks(m_tasks,m_username);
}

void MainWindow::onAddTask() {
    AddTaskDialog dlg(this);

    if (!m_voiceTaskName.isEmpty()) {
        dlg.setTaskName(m_voiceTaskName);
        m_voiceTaskName.clear();
    }

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

void MainWindow::onDeleteTask() {
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

void MainWindow::onEditTask() {
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

void MainWindow::onToggleComplete() {
    int row = ui->tableWidget->currentRow();
    if(row < 0) return;
    int id = ui->tableWidget->item(row, 0)->text().toInt();
    for(Task &t : m_tasks) {
        if(t.id == id) {
            t.completed = !t.completed;
            break;
        }
    }
    saveTasks();
    onRefreshList();
}

void MainWindow::onRefreshList() {
    QDate date = ui->calendarWidget->selectedDate();
    onDateSelected(date);
}

void MainWindow::onDateSelected(const QDate &date) {
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

void MainWindow::refreshTable(const QList<Task> &tasks) {
    ui->tableWidget->setRowCount(tasks.size());
    ui->tableWidget->setColumnCount(7);
    QStringList headers = {"ID", "名称", "开始时间", "优先级", "分类", "提醒时间", "状态"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for(int i = 0; i < tasks.size(); ++i) {
        const Task &t = tasks[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(t.name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(t.startTime.toString("yyyy-MM-dd HH:mm")));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(t.priorityToString()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(t.categoryToString()));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(t.remindTime.toString("yyyy-MM-dd HH:mm")));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(t.completed ? "✅ 已完成" : "⏳ 未完成"));
    }
    
    QHeaderView *header = ui->tableWidget->horizontalHeader();

    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    header->setSectionResizeMode(1, QHeaderView::Stretch);          // 名称
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents); // 开始时间
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // 优先级
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents); // 分类
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents); // 提醒时间
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents); // 状态

    header->setStretchLastSection(false);
    
    // ===== 更新统计面板 =====
    int total = m_tasks.size();
    int today = 0;
    int done = 0;
    QDate currentDate = QDate::currentDate();

    for(const Task &t : m_tasks) {
        if(t.startTime.date() == currentDate) today++;
        if(t.completed) done++;
    }

    ui->label_total->setText("📋 总任务数：" + QString::number(total));
    ui->label_today->setText("📅 今日任务：" + QString::number(today));
    ui->label_done->setText("✅ 已完成：" + QString::number(done));
}

void MainWindow::onTaskReminded(const Task &task)
{
    qDebug() << "===== 提醒被触发！任务：" << task.name << "=====";

    // ========== 播放自定义提示音 ==========
    QString soundPath = QApplication::applicationDirPath() + "/sounds/alert.wav";
    std::string cmd = "aplay " + soundPath.toStdString() + " > /dev/null 2>&1 &";
    system(cmd.c_str());

    // 弹窗提醒
    QMessageBox::information(this, "⏰ 任务提醒",
        QString("任务【%1】即将开始！\n开始时间：%2")
        .arg(task.name)
        .arg(task.startTime.toString("yyyy-MM-dd HH:mm")));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveTasks();
    event->accept();
}

// ========== 语音识别 ==========
void MainWindow::setupVoiceRecognizer()
{
    m_voiceRecognizer = new VoiceRecognizer(this);

    QString modelPath = QApplication::applicationDirPath() + "/model";
    if (!QFile::exists(modelPath)) {
        modelPath = QApplication::applicationDirPath() + "/../model";
    }
    if (!QFile::exists(modelPath)) {
        modelPath = "/home/code/Desktop/MySchedule--2/model";
    }

    qDebug() << "尝试加载模型:" << modelPath;

    if (!m_voiceRecognizer->init(modelPath)) {
        qDebug() << "语音识别初始化失败，请检查模型路径";
        if (ui->pushButton_Voice) {
            ui->pushButton_Voice->setEnabled(false);
            ui->pushButton_Voice->setText("❌ 语音不可用");
        }
        return;
    }

    connect(m_voiceRecognizer, &VoiceRecognizer::recognitionResult,
            this, &MainWindow::onVoiceResult);
    connect(m_voiceRecognizer, &VoiceRecognizer::statusChanged,
            this, &MainWindow::onVoiceStatus);

    if (ui->pushButton_Voice) {
        connect(ui->pushButton_Voice, &QPushButton::clicked,
                this, &MainWindow::onVoiceInput);
        ui->pushButton_Voice->setText("🎤 语音录入");
    }

    qDebug() << "语音识别已就绪";
}

void MainWindow::onVoiceInput()
{
    if (!m_voiceRecognizer) return;

    if (m_voiceRecognizer->isRecording()) {
        m_voiceRecognizer->stopRecording();
        ui->pushButton_Voice->setText("🎤 语音录入");
    } else {
        m_voiceRecognizer->startRecording(5);
        ui->pushButton_Voice->setText("⏹ 取消录音");
    }
}

void MainWindow::onVoiceResult(const QString &text)
{
    qDebug() << "语音识别结果原始数据:" << text;

    QString taskName;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("text")) {
            taskName = obj["text"].toString();
        }
    }

    if (taskName.isEmpty() && text.startsWith("{\"text\":\"")) {
        taskName = text.mid(8, text.length() - 10);
    }

    if (!taskName.isEmpty()) {
        m_voiceTaskName = taskName;
        QMessageBox::information(this, "语音识别",
            QString("识别结果：%1\n请点击「添加任务」按钮，任务名称将自动填入").arg(taskName));
    } else {
        QMessageBox::warning(this, "语音识别", "未能识别到有效语音，请重试");
    }

    if (ui->pushButton_Voice) {
        ui->pushButton_Voice->setText("🎤 语音录入");
    }
}

void MainWindow::onVoiceStatus(const QString &status)
{
    ui->statusBar->showMessage(status, 2000);
    qDebug() << "语音状态:" << status;
}
