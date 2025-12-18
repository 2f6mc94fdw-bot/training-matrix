#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(QWidget* parent = nullptr);
    ~ChangePasswordDialog();

    QString oldPassword() const { return oldPasswordEdit_->text(); }
    QString newPassword() const { return newPasswordEdit_->text(); }

private slots:
    void onAccepted();
    void onPasswordChanged();

private:
    void setupUI();
    bool validatePasswords();

private:
    QLineEdit* oldPasswordEdit_;
    QLineEdit* newPasswordEdit_;
    QLineEdit* confirmPasswordEdit_;
    QLabel* statusLabel_;
    QLabel* strengthLabel_;
};

#endif // CHANGEPASSWORDDIALOG_H
