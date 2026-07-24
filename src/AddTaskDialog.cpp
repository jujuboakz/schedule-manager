#include "AddTaskDialog.h"
#include "ui_AddTaskDialog.h"

AddTaskDialog::AddTaskDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddTaskDialog) {
    ui->setupUi(this);
    connect(ui->pushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    ui->comboBox_Priority->addItems({"楂�", "涓�", "浣�"});
    ui->comboBox_Category->addItems({"瀛︿範", "濞变箰", "鐢熸椿"});
    ui->dateTimeEdit_Start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_Remind->setDateTime(QDateTime::currentDateTime().addSecs(60));
}

AddTaskDialog::~AddTaskDialog() { delete ui; }

Task AddTaskDialog::getTask() const {
    Task t;
    t.name = ui->lineEdit_Name->text().trimmed();
    t.startTime = ui->dateTimeEdit_Start->dateTime();
    t.priority = Priority::MEDIUM;
    t.category = Category::STUDY;
    t.remindTime = ui->dateTimeEdit_Remind->dateTime();
    t.completed = false;
    return t;
}

void AddTaskDialog::setTask(const Task &t) {
    ui->lineEdit_Name->setText(t.name);
    ui->dateTimeEdit_Start->setDateTime(t.startTime);
    ui->comboBox_Priority->setCurrentText(t.priorityToString());
    ui->comboBox_Category->setCurrentText(t.categoryToString());
    ui->dateTimeEdit_Remind->setDateTime(t.remindTime);
}
