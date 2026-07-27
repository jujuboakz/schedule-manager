#include <QApplication>
#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QMetaType>
#include "LoginDialog.h"
#include "MainWindow.h"
#include "Storage.h"
#include "Task.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    //注册task类型
    qRegisterMetaType<Task>("Task");

    qDebug() << "当前工作目录:" << QDir::currentPath();

    //从users.json中读取所有用户
    auto users = Storage::loadUsers();
    qDebug() << "加载用户数:" << users.size();

    //创建登录窗口
    LoginDialog login;
    login.setUsers(users);

    if (login.exec() == QDialog::Accepted) {

        //获取最新用户数据
        users = login.getUsers();
        Storage::saveUsers(users);
        
        qDebug() << "登录成功，保存用户数据，用户数:" << users.size();

        MainWindow w(login.getUsername());
        w.show();

        //Qt事件循环开始
        return app.exec();
    }

    return 0;
}
