#include "AddTaskDialog.h"
#include "ui_AddTaskDialog.h"
#include <QDateTime>

// 采集用户输入——界面层

AddTaskDialog::AddTaskDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddTaskDialog)
{
    // 创建.ui设计的所有控件
    ui->setupUi(this);

    // 连接按钮（确定&取消）
    connect(ui->pushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);

    // 初始化下拉框（优先级&分类）
    ui->comboBox_Priority->addItems({QString::fromUtf8("高"), QString::fromUtf8("中"), QString::fromUtf8("低")});
    ui->comboBox_Category->addItems({QString::fromUtf8("学习"), QString::fromUtf8("娱乐"), QString::fromUtf8("生活")});

    // 设置默认时间
    ui->dateTimeEdit_Start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_Remind->setDateTime(QDateTime::currentDateTime().addSecs(60));
}

AddTaskDialog::~AddTaskDialog()
{
    // 释放Qt Designer构建的ui对象
    delete ui;
}

Task AddTaskDialog::getTask() const // 新增任务
{
    Task t;
    t.id = 0; // id由主窗口生成，这里设为0
    t.name = ui->lineEdit_Name->text().trimmed();
    t.startTime = ui->dateTimeEdit_Start->dateTime();
    
    // 使用反向转换函数，正确读取下拉框的值
    t.priority = Task::stringToPriority(ui->comboBox_Priority->currentText());
    t.category = Task::stringToCategory(ui->comboBox_Category->currentText());
    
    t.remindTime = ui->dateTimeEdit_Remind->dateTime();
    t.completed = false; // 新创建任务默认未完成
    return t;
}

void AddTaskDialog::setTask(const Task &t) // 修改任务
{
    ui->lineEdit_Name->setText(t.name);
    ui->dateTimeEdit_Start->setDateTime(t.startTime);
    
    // 枚举转字符串
    ui->comboBox_Priority->setCurrentText(t.priorityToString());
    ui->comboBox_Category->setCurrentText(t.categoryToString());
    
    ui->dateTimeEdit_Remind->setDateTime(t.remindTime);
}

// 语音识别新增接口
void AddTaskDialog::setTaskName(const QString &name)
{
    ui->lineEdit_Name->setText(name);
}