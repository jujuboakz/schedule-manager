#include "RegisterDialog.h"
#include "ui_RegisterDialog.h"
#include <QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);
    connect(ui->pushButton_Confirm, &QPushButton::clicked, this, &RegisterDialog::onConfirmClicked);
}

RegisterDialog::~RegisterDialog() { delete ui; }

QString RegisterDialog::getUsername() const { return ui->lineEdit_Username->text().trimmed(); }
QString RegisterDialog::getPassword() const { return ui->lineEdit_Password->text().trimmed(); }

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
