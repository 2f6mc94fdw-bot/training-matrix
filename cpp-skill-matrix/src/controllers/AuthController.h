#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QString>

class AuthController
{
public:
    AuthController();
    ~AuthController();

    bool login(const QString& username, const QString& password);
    void logout();
    bool changePassword(const QString& oldPassword, const QString& newPassword);
    static int lockoutSecondsRemaining(const QString& username);
};

#endif // AUTHCONTROLLER_H
