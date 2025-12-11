#include "EngineerRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

EngineerRepository::EngineerRepository() : lastError_("") {}
EngineerRepository::~EngineerRepository() {}

QList<Engineer> EngineerRepository::findAll()
{
    QList<Engineer> engineers;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("EngineerRepository", lastError_);
        return engineers;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT id, name, shift, created_at, updated_at FROM engineers ORDER BY name")) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "findAll failed: " + lastError_);
        return engineers;
    }

    while (query.next()) {
        Engineer engineer;
        engineer.setId(query.value(0).toString());
        engineer.setName(query.value(1).toString());
        engineer.setShift(query.value(2).toString());
        engineer.setCreatedAt(query.value(3).toDateTime());
        engineer.setUpdatedAt(query.value(4).toDateTime());
        engineers.append(engineer);
    }

    Logger::instance().debug("EngineerRepository", QString("Found %1 engineers").arg(engineers.size()));
    return engineers;
}

QList<Engineer> EngineerRepository::findByShift(const QString& shift)
{
    QList<Engineer> engineers;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return engineers;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, shift, created_at, updated_at FROM engineers WHERE shift = ? ORDER BY name");
    query.addBindValue(shift);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "findByShift failed: " + lastError_);
        return engineers;
    }

    while (query.next()) {
        Engineer engineer;
        engineer.setId(query.value(0).toString());
        engineer.setName(query.value(1).toString());
        engineer.setShift(query.value(2).toString());
        engineer.setCreatedAt(query.value(3).toDateTime());
        engineer.setUpdatedAt(query.value(4).toDateTime());
        engineers.append(engineer);
    }

    Logger::instance().debug("EngineerRepository", QString("Found %1 engineers in shift %2").arg(engineers.size()).arg(shift));
    return engineers;
}

Engineer EngineerRepository::findById(const QString& id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return Engineer();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, shift, created_at, updated_at FROM engineers WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "findById failed: " + lastError_);
        return Engineer();
    }

    if (query.next()) {
        Engineer engineer;
        engineer.setId(query.value(0).toString());
        engineer.setName(query.value(1).toString());
        engineer.setShift(query.value(2).toString());
        engineer.setCreatedAt(query.value(3).toDateTime());
        engineer.setUpdatedAt(query.value(4).toDateTime());
        return engineer;
    }

    return Engineer();
}

bool EngineerRepository::save(Engineer& engineer)
{
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    // Generate ID if not set
    if (engineer.id().isEmpty()) {
        engineer.setId(Crypto::generateId("eng"));
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO engineers (id, name, shift, created_at, updated_at) "
                  "VALUES (?, ?, ?, GETDATE(), GETDATE())");
    query.addBindValue(engineer.id());
    query.addBindValue(engineer.name());
    query.addBindValue(engineer.shift());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "save failed: " + lastError_);
        return false;
    }

    Logger::instance().info("EngineerRepository", "Engineer saved: " + engineer.name());
    return true;
}

bool EngineerRepository::update(const Engineer& engineer)
{
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE engineers SET name = ?, shift = ?, updated_at = GETDATE() WHERE id = ?");
    query.addBindValue(engineer.name());
    query.addBindValue(engineer.shift());
    query.addBindValue(engineer.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "update failed: " + lastError_);
        return false;
    }

    Logger::instance().info("EngineerRepository", "Engineer updated: " + engineer.name());
    return true;
}

bool EngineerRepository::remove(const QString& id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM engineers WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("EngineerRepository", "remove failed: " + lastError_);
        return false;
    }

    Logger::instance().info("EngineerRepository", "Engineer removed: " + id);
    return true;
}
