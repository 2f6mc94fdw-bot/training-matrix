#include "UserRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

UserRepository::UserRepository() : lastError_("") {}
UserRepository::~UserRepository() {}

QList<User> UserRepository::findAll()
{
    lastError_.clear();  // Clear any previous errors
    QList<User> users;
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("UserRepository", lastError_);
        return users;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, password, role, engineer_id, created_at, updated_at "
                  "FROM users ORDER BY username");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "findAll failed: " + lastError_);
        return users;
    }

    while (query.next()) {
        User user;
        user.setId(query.value(0).toString());
        user.setUsername(query.value(1).toString());
        user.setPassword(query.value(2).toString());
        user.setRole(query.value(3).toString());
        user.setEngineerId(query.value(4).toString());
        user.setCreatedAt(query.value(5).toDateTime());
        user.setUpdatedAt(query.value(6).toDateTime());
        users.append(user);
    }

    Logger::instance().debug("UserRepository", QString("Found %1 users").arg(users.size()));
    return users;
}

User UserRepository::findById(const QString& id)
{
    lastError_.clear();  // Clear any previous errors
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("UserRepository", lastError_);
        return User();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, password, role, engineer_id, created_at, updated_at "
                  "FROM users WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "findById failed: " + lastError_);
        return User();
    }

    if (query.next()) {
        User user;
        user.setId(query.value(0).toString());
        user.setUsername(query.value(1).toString());
        user.setPassword(query.value(2).toString());
        user.setRole(query.value(3).toString());
        user.setEngineerId(query.value(4).toString());
        user.setCreatedAt(query.value(5).toDateTime());
        user.setUpdatedAt(query.value(6).toDateTime());

        Logger::instance().debug("UserRepository", "Found user by ID: " + id);
        return user;
    }

    Logger::instance().debug("UserRepository", "User not found by ID: " + id);
    return User();
}

User UserRepository::findByUsername(const QString& username)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("UserRepository", lastError_);
        return User();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, password, role, engineer_id, created_at, updated_at "
                  "FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "findByUsername failed: " + lastError_);
        return User();
    }

    if (query.next()) {
        User user;
        user.setId(query.value(0).toString());
        user.setUsername(query.value(1).toString());
        user.setPassword(query.value(2).toString());
        user.setRole(query.value(3).toString());
        user.setEngineerId(query.value(4).toString());
        user.setCreatedAt(query.value(5).toDateTime());
        user.setUpdatedAt(query.value(6).toDateTime());

        Logger::instance().debug("UserRepository", "Found user: " + username);
        return user;
    }

    Logger::instance().debug("UserRepository", "User not found: " + username);
    return User();
}

bool UserRepository::save(User& user)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (id, username, password, role, engineer_id, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, GETDATE(), GETDATE())");
    query.addBindValue(user.id());
    query.addBindValue(user.username());
    query.addBindValue(user.password());
    query.addBindValue(user.role());
    query.addBindValue(user.engineerId().isEmpty() ? QVariant() : user.engineerId());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "save failed: " + lastError_);
        return false;
    }

    Logger::instance().info("UserRepository", "User saved: " + user.username());
    return true;
}

bool UserRepository::update(const User& user)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE users SET username = ?, role = ?, engineer_id = ?, updated_at = GETDATE() "
                  "WHERE id = ?");
    query.addBindValue(user.username());
    query.addBindValue(user.role());
    query.addBindValue(user.engineerId().isEmpty() ? QVariant() : user.engineerId());
    query.addBindValue(user.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "update failed: " + lastError_);
        return false;
    }

    Logger::instance().info("UserRepository", "User updated: " + user.username());
    return true;
}

bool UserRepository::remove(const QString& id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "remove failed: " + lastError_);
        return false;
    }

    Logger::instance().info("UserRepository", "User removed: " + id);
    return true;
}

bool UserRepository::updatePassword(const QString& id, const QString& newPasswordHash)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE users SET password = ?, updated_at = GETDATE() WHERE id = ?");
    query.addBindValue(newPasswordHash);
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("UserRepository", "updatePassword failed: " + lastError_);
        return false;
    }

    Logger::instance().info("UserRepository", "Password updated for user: " + id);
    return true;
}

User UserRepository::authenticate(const QString& username, const QString& passwordHash)
{
    User user = findByUsername(username);

    if (!user.isValid()) {
        Logger::instance().warning("UserRepository", "Authentication failed - user not found: " + username);
        return User();
    }

    if (user.password() != passwordHash) {
        Logger::instance().warning("UserRepository", "Authentication failed - invalid password for: " + username);
        return User();
    }

    Logger::instance().info("UserRepository", "Authentication successful: " + username);
    return user;
}
