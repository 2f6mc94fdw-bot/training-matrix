#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

    QString username() const;
    QString password() const;

private slots:
    void onLoginClicked();
    void onCancelClicked();

private:
    void setupUI();
    bool validateInput();
    bool attemptLogin();

private:
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QPushButton* loginButton_;
    QPushButton* cancelButton_;
    QLabel* statusLabel_;
};

#endif // LOGINDIALOG_H
