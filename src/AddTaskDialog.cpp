#include "AddTaskDialog.h"
#include "ui_AddTaskDialog.h"
#include <QDateTime>

AddTaskDialog::AddTaskDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddTaskDialog)
{
    ui->setupUi(this);

    // 连接按钮
    connect(ui->pushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);

    // 初始化下拉框 (改用 QString::fromUtf8 绝对防止乱码)
    ui->comboBox_Priority->addItems({QString::fromUtf8("高"), QString::fromUtf8("中"), QString::fromUtf8("低")});
    ui->comboBox_Category->addItems({QString::fromUtf8("学习"), QString::fromUtf8("娱乐"), QString::fromUtf8("生活")});

    // 设置默认时间
    ui->dateTimeEdit_Start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_Remind->setDateTime(QDateTime::currentDateTime().addSecs(60));
}

AddTaskDialog::~AddTaskDialog()
{
    delete ui;
}

Task AddTaskDialog::getTask() const
{
    Task t;
    t.id = 0; // id由主窗口生成，这里设为0
    t.name = ui->lineEdit_Name->text().trimmed();
    t.startTime = ui->dateTimeEdit_Start->dateTime();
    
    // 使用反向转换函数，正确读取下拉框的值
    t.priority = Task::stringToPriority(ui->comboBox_Priority->currentText());
    t.category = Task::stringToCategory(ui->comboBox_Category->currentText());
    
    t.remindTime = ui->dateTimeEdit_Remind->dateTime();
    t.completed = false; // 使用原版 Task.h 里的变量名 completed
    return t;
}

void AddTaskDialog::setTask(const Task &t)
{
    ui->lineEdit_Name->setText(t.name);
    ui->dateTimeEdit_Start->setDateTime(t.startTime);
    
    // 使用原版 Task.h 里的函数调用方式 t.priorityToString()
    ui->comboBox_Priority->setCurrentText(t.priorityToString());
    ui->comboBox_Category->setCurrentText(t.categoryToString());
    
    ui->dateTimeEdit_Remind->setDateTime(t.remindTime);
}

// 【新增】添加这个函数，解决之前 MainWindow 调用 setTaskName 报错的问题
void AddTaskDialog::setTaskName(const QString &name)
{
    ui->lineEdit_Name->setText(name);
}