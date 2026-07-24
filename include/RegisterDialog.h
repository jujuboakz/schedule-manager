#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>

namespace Ui { class RegisterDialog; }

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
    QString getUsername() const;
    QString getPassword() const;

private slots:
    void onConfirmClicked();

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTER_DIALOG_H
