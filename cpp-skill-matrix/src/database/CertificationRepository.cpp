#include "CertificationRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

CertificationRepository::CertificationRepository() : lastError_("") {}
CertificationRepository::~CertificationRepository() {}

QList<Certification> CertificationRepository::findAll()
{
    lastError_.clear();
    QList<Certification> certifications;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CertificationRepository", lastError_);
        return certifications;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, name, date_earned, expiry_date, created_at "
                  "FROM certifications ORDER BY engineer_id, date_earned DESC");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CertificationRepository", "findAll failed: " + lastError_);
        return certifications;
    }

    while (query.next()) {
        Certification cert;
        cert.setId(query.value(0).toInt());
        cert.setEngineerId(query.value(1).toString());
        cert.setName(query.value(2).toString());
        cert.setDateEarned(query.value(3).toDate());
        cert.setExpiryDate(query.value(4).toDate());
        cert.setCreatedAt(query.value(5).toDateTime());
        certifications.append(cert);
    }

    Logger::instance().debug("CertificationRepository", QString("Found %1 certifications").arg(certifications.size()));
    return certifications;
}

QList<Certification> CertificationRepository::findByEngineer(const QString& engineerId)
{
    lastError_.clear();
    QList<Certification> certifications;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CertificationRepository", lastError_);
        return certifications;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, name, date_earned, expiry_date, created_at "
                  "FROM certifications WHERE engineer_id = ? ORDER BY date_earned DESC");
    query.addBindValue(engineerId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CertificationRepository", "findByEngineer failed: " + lastError_);
        return certifications;
    }

    while (query.next()) {
        Certification cert;
        cert.setId(query.value(0).toInt());
        cert.setEngineerId(query.value(1).toString());
        cert.setName(query.value(2).toString());
        cert.setDateEarned(query.value(3).toDate());
        cert.setExpiryDate(query.value(4).toDate());
        cert.setCreatedAt(query.value(5).toDateTime());
        certifications.append(cert);
    }

    Logger::instance().debug("CertificationRepository",
        QString("Found %1 certifications for engineer %2").arg(certifications.size()).arg(engineerId));
    return certifications;
}

bool CertificationRepository::save(Certification& certification)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CertificationRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);

    if (certification.id() > 0) {
        // Update existing certification
        query.prepare("UPDATE certifications SET engineer_id = ?, name = ?, date_earned = ?, expiry_date = ? "
                     "WHERE id = ?");
        query.addBindValue(certification.engineerId());
        query.addBindValue(certification.name());
        query.addBindValue(certification.dateEarned());
        query.addBindValue(certification.expiryDate().isValid() ? QVariant(certification.expiryDate()) : QVariant());
        query.addBindValue(certification.id());

        if (!query.exec()) {
            lastError_ = query.lastError().text();
            Logger::instance().error("CertificationRepository", "update failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CertificationRepository", "Certification updated: " + certification.name());
        return true;
    } else {
        // Insert new certification
        query.prepare("INSERT INTO certifications (engineer_id, name, date_earned, expiry_date, created_at) "
                     "VALUES (?, ?, ?, ?, GETDATE())");
        query.addBindValue(certification.engineerId());
        query.addBindValue(certification.name());
        query.addBindValue(certification.dateEarned());
        query.addBindValue(certification.expiryDate().isValid() ? QVariant(certification.expiryDate()) : QVariant());

        if (!query.exec()) {
            lastError_ = query.lastError().text();
            Logger::instance().error("CertificationRepository", "insert failed: " + lastError_);
            return false;
        }

        // Get auto-generated ID
        QVariant lastId = query.lastInsertId();
        if (lastId.isValid()) {
            certification.setId(lastId.toInt());
        }

        Logger::instance().info("CertificationRepository", "Certification created: " + certification.name());
        return true;
    }
}

bool CertificationRepository::remove(int id)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CertificationRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM certifications WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CertificationRepository", "remove failed: " + lastError_);
        return false;
    }

    Logger::instance().info("CertificationRepository", QString("Certification removed: %1").arg(id));
    return true;
}
