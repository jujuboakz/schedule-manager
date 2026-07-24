#include <QApplication>
#include <QMetaType>
#include "LoginDialog.h"
#include "MainWindow.h"
#include "Storage.h"
#include "Task.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<Task>("Task");

    auto users = Storage::loadUsers();

    LoginDialog login;
    login.setUsers(users);

    if(login.exec() == QDialog::Accepted) {
        Storage::saveUsers(users);
        MainWindow w(login.getUsername());
        w.show();
        return app.exec();
    }

    return 0;
}