#include "NotificationRepository.h"
#include "DatabaseManager.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

NotificationRepository::NotificationRepository() : lastError_("") {}
NotificationRepository::~NotificationRepository() {}

bool NotificationRepository::ensureSchema()
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return false;
    }

    if (schemaEnsured_) {
        return true;
    }

    schemaEnsured_ = true;
    return true;
}

bool NotificationRepository::createForEngineer(const QString& engineerId,
                                               const QString& title,
                                               const QString& message,
                                               const QString& createdByUserId,
                                               const QString& createdByName)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return false;
    }
    if (!ensureSchema()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO notifications (id, engineer_id, recipient_user_id, title, message, is_read, created_at, created_by_user_id, created_by_name) "
        "VALUES (?, ?, NULL, ?, ?, 0, GETDATE(), ?, ?)"
    );
    query.addBindValue(Crypto::generateId("note"));
    query.addBindValue(engineerId);
    query.addBindValue(title);
    query.addBindValue(message);
    query.addBindValue(createdByUserId.isEmpty() ? QVariant() : createdByUserId);
    query.addBindValue(createdByName.isEmpty() ? QVariant() : createdByName);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("NotificationRepository", "createForEngineer failed: " + lastError_);
        return false;
    }

    return true;
}

bool NotificationRepository::createForRole(const QString& role,
                                           const QString& title,
                                           const QString& message,
                                           const QString& createdByUserId,
                                           const QString& createdByName)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return false;
    }
    if (!ensureSchema()) {
        return false;
    }

    QSqlQuery usersQuery(db);
    usersQuery.prepare("SELECT id FROM users WHERE role = ?");
    usersQuery.addBindValue(role);
    if (!usersQuery.exec()) {
        lastError_ = usersQuery.lastError().text();
        Logger::instance().error("NotificationRepository", "createForRole user lookup failed: " + lastError_);
        return false;
    }

    bool hasRecipients = false;
    while (usersQuery.next()) {
        hasRecipients = true;
        const QString recipientUserId = usersQuery.value(0).toString();
        QSqlQuery insertQuery(db);
        insertQuery.prepare(
            "INSERT INTO notifications (id, engineer_id, recipient_user_id, title, message, is_read, created_at, created_by_user_id, created_by_name) "
            "VALUES (?, ?, ?, ?, ?, 0, GETDATE(), ?, ?)"
        );
        insertQuery.addBindValue(Crypto::generateId("note"));
        insertQuery.addBindValue(QVariant());
        insertQuery.addBindValue(recipientUserId);
        insertQuery.addBindValue(title);
        insertQuery.addBindValue(message);
        insertQuery.addBindValue(createdByUserId.isEmpty() ? QVariant() : createdByUserId);
        insertQuery.addBindValue(createdByName.isEmpty() ? QVariant() : createdByName);

        if (!insertQuery.exec()) {
            lastError_ = insertQuery.lastError().text();
            Logger::instance().error("NotificationRepository", "createForRole insert failed: " + lastError_);
            return false;
        }
    }

    return hasRecipients;
}

QList<EngineerNotification> NotificationRepository::findByEngineer(const QString& engineerId,
                                                                   bool onlyUnread,
                                                                   int limit)
{
    lastError_.clear();
    QList<EngineerNotification> notifications;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return notifications;
    }
    if (!ensureSchema()) {
        return notifications;
    }

    QSqlQuery query(db);
    QString sql =
        "SELECT TOP (?) id, engineer_id, title, message, is_read, created_at, read_at, created_by_user_id, created_by_name "
        "FROM notifications WHERE engineer_id = ? ";
    if (onlyUnread) {
        sql += "AND is_read = 0 ";
    }
    sql += "ORDER BY created_at DESC";

    query.prepare(sql);
    query.addBindValue(limit);
    query.addBindValue(engineerId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("NotificationRepository", "findByEngineer failed: " + lastError_);
        return notifications;
    }

    while (query.next()) {
        EngineerNotification n;
        n.id = query.value(0).toString();
        n.engineerId = query.value(1).toString();
        n.title = query.value(2).toString();
        n.message = query.value(3).toString();
        n.isRead = query.value(4).toBool();
        n.createdAt = query.value(5).toDateTime();
        n.readAt = query.value(6).toDateTime();
        n.createdByUserId = query.value(7).toString();
        n.createdByName = query.value(8).toString();
        notifications.append(n);
    }

    return notifications;
}

QList<EngineerNotification> NotificationRepository::findByRecipientUser(const QString& userId,
                                                                        bool onlyUnread,
                                                                        int limit)
{
    lastError_.clear();
    QList<EngineerNotification> notifications;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return notifications;
    }
    if (!ensureSchema()) {
        return notifications;
    }

    QSqlQuery query(db);
    QString sql =
        "SELECT TOP (?) id, engineer_id, title, message, is_read, created_at, read_at, created_by_user_id, created_by_name "
        "FROM notifications WHERE recipient_user_id = ? ";
    if (onlyUnread) {
        sql += "AND is_read = 0 ";
    }
    sql += "ORDER BY created_at DESC";

    query.prepare(sql);
    query.addBindValue(limit);
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("NotificationRepository", "findByRecipientUser failed: " + lastError_);
        return notifications;
    }

    while (query.next()) {
        EngineerNotification n;
        n.id = query.value(0).toString();
        n.engineerId = query.value(1).toString();
        n.title = query.value(2).toString();
        n.message = query.value(3).toString();
        n.isRead = query.value(4).toBool();
        n.createdAt = query.value(5).toDateTime();
        n.readAt = query.value(6).toDateTime();
        n.createdByUserId = query.value(7).toString();
        n.createdByName = query.value(8).toString();
        notifications.append(n);
    }

    return notifications;
}

bool NotificationRepository::markAllReadForEngineer(const QString& engineerId)
{
    lastError_.clear();

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return false;
    }
    if (!ensureSchema()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE notifications "
        "SET is_read = 1, read_at = GETDATE() "
        "WHERE engineer_id = ? AND is_read = 0"
    );
    query.addBindValue(engineerId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("NotificationRepository", "markAllReadForEngineer failed: " + lastError_);
        return false;
    }

    return true;
}

bool NotificationRepository::markAllReadForUser(const QString& userId)
{
    lastError_.clear();

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("NotificationRepository", lastError_);
        return false;
    }
    if (!ensureSchema()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE notifications "
        "SET is_read = 1, read_at = GETDATE() "
        "WHERE recipient_user_id = ? AND is_read = 0"
    );
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("NotificationRepository", "markAllReadForUser failed: " + lastError_);
        return false;
    }

    return true;
}
