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
    
    qRegisterMetaType<Task>("Task");

    qDebug() << "当前工作目录:" << QDir::currentPath();

    auto users = Storage::loadUsers();
    qDebug() << "加载用户数:" << users.size();

    LoginDialog login;
    login.setUsers(users);

    if (login.exec() == QDialog::Accepted) {
        users = login.getUsers();
        Storage::saveUsers(users);
        qDebug() << "登录成功，保存用户数据，用户数:" << users.size();

        MainWindow w(login.getUsername());
        w.show();
        return app.exec();
    }

    return 0;
}
