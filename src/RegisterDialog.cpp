#include "RegisterDialog.h"
#include "ui_RegisterDialog.h"
#include <QMessageBox>

// 采集用户名和密码，返回给LoginDialog

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);
    
    // 确认后进行注册检查
    connect(ui->pushButton_Confirm, &QPushButton::clicked, this, &RegisterDialog::onConfirmClicked);
}

RegisterDialog::~RegisterDialog() { delete ui; }

QString RegisterDialog::getUsername() const { return ui->lineEdit_Username->text().trimmed(); }

// 直接返回密码（还未加密）
QString RegisterDialog::getPassword() const { return ui->lineEdit_Password->text().trimmed(); }

// 密码合法性检查
void RegisterDialog::onConfirmClicked() {
    if(getUsername().isEmpty() || getPassword().isEmpty()) {
        QMessageBox::warning(this, u8"错误", u8"用户名或密码不能为空");
        return;
    }
    if(getPassword().length() < 6) {
        QMessageBox::warning(this, u8"错误", u8"密码长度不能少于6位");
        return;
    }
    accept();
}

// 不涉及保存用户