#include "SnapshotRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SnapshotRepository::SnapshotRepository() : lastError_("") {}
SnapshotRepository::~SnapshotRepository() {}

QList<Snapshot> SnapshotRepository::findAll(int limit)
{
    lastError_.clear();
    QList<Snapshot> snapshots;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("SnapshotRepository", lastError_);
        return snapshots;
    }

    QSqlQuery query(db);
    query.prepare("SELECT TOP (?) id, description, timestamp, data, created_at "
                  "FROM snapshots ORDER BY timestamp DESC");
    query.addBindValue(limit);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("SnapshotRepository", "findAll failed: " + lastError_);
        return snapshots;
    }

    while (query.next()) {
        Snapshot snapshot;
        snapshot.setId(query.value(0).toString());
        snapshot.setDescription(query.value(1).toString());
        snapshot.setTimestamp(query.value(2).toDateTime());
        snapshot.setData(query.value(3).toString());
        snapshot.setCreatedAt(query.value(4).toDateTime());
        snapshots.append(snapshot);
    }

    Logger::instance().debug("SnapshotRepository", QString("Found %1 snapshots").arg(snapshots.size()));
    return snapshots;
}

Snapshot SnapshotRepository::findById(const QString& id)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("SnapshotRepository", lastError_);
        return Snapshot();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, description, timestamp, data, created_at "
                  "FROM snapshots WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("SnapshotRepository", "findById failed: " + lastError_);
        return Snapshot();
    }

    if (query.next()) {
        Snapshot snapshot;
        snapshot.setId(query.value(0).toString());
        snapshot.setDescription(query.value(1).toString());
        snapshot.setTimestamp(query.value(2).toDateTime());
        snapshot.setData(query.value(3).toString());
        snapshot.setCreatedAt(query.value(4).toDateTime());

        Logger::instance().debug("SnapshotRepository", "Found snapshot: " + id);
        return snapshot;
    }

    Logger::instance().debug("SnapshotRepository", "Snapshot not found: " + id);
    return Snapshot();
}

bool SnapshotRepository::save(Snapshot& snapshot)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("SnapshotRepository", lastError_);
        return false;
    }

    // Generate ID if not already set
    if (snapshot.id().isEmpty()) {
        snapshot.setId(Crypto::generateId("snapshot"));
    }

    // Set timestamp if not already set
    if (!snapshot.timestamp().isValid()) {
        snapshot.setTimestamp(QDateTime::currentDateTime());
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO snapshots (id, description, timestamp, data, created_at) "
                  "VALUES (?, ?, ?, ?, GETDATE())");
    query.addBindValue(snapshot.id());
    query.addBindValue(snapshot.description().isEmpty() ? QVariant() : snapshot.description());
    query.addBindValue(snapshot.timestamp());
    query.addBindValue(snapshot.data());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("SnapshotRepository", "save failed: " + lastError_);
        return false;
    }

    Logger::instance().info("SnapshotRepository", "Snapshot saved: " + snapshot.description());
    return true;
}

bool SnapshotRepository::remove(const QString& id)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("SnapshotRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM snapshots WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("SnapshotRepository", "remove failed: " + lastError_);
        return false;
    }

    Logger::instance().info("SnapshotRepository", "Snapshot removed: " + id);
    return true;
}
