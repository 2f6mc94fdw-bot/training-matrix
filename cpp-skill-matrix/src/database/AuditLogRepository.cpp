#include "AuditLogRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

AuditLogRepository::AuditLogRepository() : lastError_("") {}
AuditLogRepository::~AuditLogRepository() {}

QList<AuditLog> AuditLogRepository::findAll(int limit)
{
    lastError_.clear();
    QList<AuditLog> logs;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AuditLogRepository", lastError_);
        return logs;
    }

    QSqlQuery query(db);
    query.prepare("SELECT TOP (?) id, timestamp, user_id, action, details, created_at "
                  "FROM audit_logs ORDER BY timestamp DESC");
    query.addBindValue(limit);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AuditLogRepository", "findAll failed: " + lastError_);
        return logs;
    }

    while (query.next()) {
        AuditLog log;
        log.setId(query.value(0).toString());
        log.setTimestamp(query.value(1).toDateTime());
        log.setUserId(query.value(2).toString());
        log.setAction(query.value(3).toString());
        log.setDetails(query.value(4).toString());
        log.setCreatedAt(query.value(5).toDateTime());
        logs.append(log);
    }

    Logger::instance().debug("AuditLogRepository", QString("Found %1 audit log entries").arg(logs.size()));
    return logs;
}

QList<AuditLog> AuditLogRepository::findByUser(const QString& userId)
{
    lastError_.clear();
    QList<AuditLog> logs;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AuditLogRepository", lastError_);
        return logs;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, timestamp, user_id, action, details, created_at "
                  "FROM audit_logs WHERE user_id = ? ORDER BY timestamp DESC");
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AuditLogRepository", "findByUser failed: " + lastError_);
        return logs;
    }

    while (query.next()) {
        AuditLog log;
        log.setId(query.value(0).toString());
        log.setTimestamp(query.value(1).toDateTime());
        log.setUserId(query.value(2).toString());
        log.setAction(query.value(3).toString());
        log.setDetails(query.value(4).toString());
        log.setCreatedAt(query.value(5).toDateTime());
        logs.append(log);
    }

    Logger::instance().debug("AuditLogRepository",
        QString("Found %1 audit log entries for user %2").arg(logs.size()).arg(userId));
    return logs;
}

bool AuditLogRepository::save(AuditLog& log)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AuditLogRepository", lastError_);
        return false;
    }

    // Generate ID if not already set
    if (log.id().isEmpty()) {
        log.setId(Crypto::generateId("audit"));
    }

    // Set timestamp if not already set
    if (!log.timestamp().isValid()) {
        log.setTimestamp(QDateTime::currentDateTime());
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO audit_logs (id, timestamp, user_id, action, details, created_at) "
                  "VALUES (?, ?, ?, ?, ?, GETDATE())");
    query.addBindValue(log.id());
    query.addBindValue(log.timestamp());
    query.addBindValue(log.userId().isEmpty() ? QVariant() : log.userId());
    query.addBindValue(log.action());
    query.addBindValue(log.details().isEmpty() ? QVariant() : log.details());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AuditLogRepository", "save failed: " + lastError_);
        return false;
    }

    Logger::instance().info("AuditLogRepository",
        QString("Audit log saved: %1 - %2").arg(log.action()).arg(log.details()));
    return true;
}
