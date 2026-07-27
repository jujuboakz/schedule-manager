#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "RegisterDialog.h"
#include "Storage.h"
#include <QMessageBox>
#include <QCryptographicHash>

// 身份认证入口
// 登录验证
// 新用户注册

// 创建登录窗口
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);

    // 将按钮和槽函数连接起来
    // 按下按钮后调用相关函数
    connect(ui->pushButton_Login, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->pushButton_Register, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
}

LoginDialog::~LoginDialog() { delete ui; }

// 把users.json交给LoginDialog
void LoginDialog::setUsers(const QMap<QString, QString> &users) {
    m_users = users;
}

QString LoginDialog::getUsername() const {
    return ui->lineEdit_Username->text();
}

void LoginDialog::onLoginClicked() {

    // 读取输入框并去掉空格（trimmed）
    QString name = ui->lineEdit_Username->text().trimmed();
    QString pwd = ui->lineEdit_Password->text().trimmed();

    // 判空
    if(name.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "错误", "用户名或密码不能为空");
        return;
    }

    // 密码的哈希保存
    QString hash = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex();
    
    //验证（用户存在 + 哈希一致）
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
        
        if(!m_users.contains(name)) { // 用户名可用
            QString hash = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex();
            m_users[name] = hash;
            
            qDebug() << "注册后 m_users 大小:" << m_users.size();
            qDebug() << "m_users 内容:" << m_users;
            
            bool result = Storage::saveUsers(m_users); // 写users.json
            qDebug() << "Storage::saveUsers 返回:" << result;
            
            if(result) {
                // 立即读取验证（调试代码）
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
