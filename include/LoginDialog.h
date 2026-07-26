#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUsername() const;
    void setUsers(const QMap<QString, QString> &users);
    QMap<QString, QString> getUsers() const { return m_users; }

private slots:
    void onLoginClicked();
    void onRegisterClicked();

signals:
    void loginSuccess(const QString &username);

private:
    Ui::LoginDialog *ui;
    QMap<QString, QString> m_users;
};

#endif // LOGIN_DIALOG_H
