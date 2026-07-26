#include "LoginDialog.h"
#include "ui_LoginDialog.h"          // ← 这里改了
#include "RegisterDialog.h"
#include "Storage.h"
#include <QMessageBox>
#include <QCryptographicHash>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    connect(ui->pushButton_Login, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->pushButton_Register, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::setUsers(const QMap<QString, QString> &users) {
    m_users = users;
}

QString LoginDialog::getUsername() const {
    return ui->lineEdit_Username->text();
}

void LoginDialog::onLoginClicked() {
    QString name = ui->lineEdit_Username->text().trimmed();
    QString pwd = ui->lineEdit_Password->text().trimmed();
    if(name.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "错误", "用户名或密码不能为空");
        return;
    }
    QString hash = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex();
    if(m_users.contains(name) && m_users[name] == hash) {
        accept();
    } else {
        QMessageBox::warning(this, "错误", "用户名或密码错误");
    }
}

void LoginDialog::onRegisterClicked() {
    RegisterDialog reg(this);
    if(reg.exec() == QDialog::Accepted) {
        QString name = reg.getUsername();
        QString pwd = reg.getPassword();
        
        qDebug() << "=== 注册流程开始 ===";
        qDebug() << "用户名:" << name;
        qDebug() << "注册前 m_users 大小:" << m_users.size();
        
        if(!m_users.contains(name)) {
            QString hash = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex();
            m_users[name] = hash;
            
            qDebug() << "注册后 m_users 大小:" << m_users.size();
            qDebug() << "m_users 内容:" << m_users;
            
            bool result = Storage::saveUsers(m_users);
            qDebug() << "Storage::saveUsers 返回:" << result;
            
            if(result) {
                // 立即读取验证
                auto loaded = Storage::loadUsers();
                qDebug() << "写入后重新读取，用户数:" << loaded.size();
                QMessageBox::information(this, "成功", "注册成功，请登录");
            } else {
                QMessageBox::warning(this, "错误", "保存用户数据失败");
            }
        } else {
            QMessageBox::warning(this, "错误", "用户名已存在");
        }
    }
}
