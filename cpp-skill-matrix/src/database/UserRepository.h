#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../models/User.h"
#include <QList>
#include <QString>

class QSqlDatabase;

/**
 * @brief Repository for User CRUD operations
 */
class UserRepository
{
public:
    UserRepository();
    ~UserRepository();

    // CRUD operations
    QList<User> findAll();
    User findById(const QString& id);
    User findByUsername(const QString& username);
    bool save(User& user);
    bool update(const User& user);
    bool remove(const QString& id);
    bool updatePassword(const QString& id, const QString& newPasswordHash);

    // Authentication
    User authenticate(const QString& username, const QString& passwordHash);

    // Error handling
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // USERREPOSITORY_H
