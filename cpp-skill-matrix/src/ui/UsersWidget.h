#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "../database/UserRepository.h"
#include "../database/EngineerRepository.h"

class UsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget* parent = nullptr);
    ~UsersWidget();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onResetPasswordClicked();
    void onRefreshClicked();
    void onTableDoubleClicked(int row, int column);

private:
    void setupUI();
    void loadUsers();
    void showUserDialog(const User* user = nullptr);
    void showPasswordResetDialog(const User& user);
    QString getEngineerName(const QString& engineerId);

private:
    QTableWidget* tableWidget_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* resetPasswordButton_;
    QPushButton* refreshButton_;

    UserRepository userRepository_;
    EngineerRepository engineerRepository_;
};

#endif // USERSWIDGET_H
