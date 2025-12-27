#include "CertificationController.h"
#include "../database/CertificationRepository.h"
#include "../utils/Logger.h"

CertificationController::CertificationController() : lastError_("") {}
CertificationController::~CertificationController() {}

QList<Certification> CertificationController::getAllCertifications()
{
    lastError_.clear();
    CertificationRepository repo;
    return repo.findAll();
}

QList<Certification> CertificationController::getCertificationsByEngineer(const QString& engineerId)
{
    lastError_.clear();
    CertificationRepository repo;
    return repo.findByEngineer(engineerId);
}

QList<Certification> CertificationController::getExpiredCertifications()
{
    QList<Certification> expired;
    QList<Certification> all = getAllCertifications();
    QDate today = QDate::currentDate();

    for (const Certification& cert : all) {
        if (cert.isExpired()) {
            expired.append(cert);
        }
    }

    return expired;
}

QList<Certification> CertificationController::getExpiringCertifications(int daysThreshold)
{
    QList<Certification> expiring;
    QList<Certification> all = getAllCertifications();

    for (const Certification& cert : all) {
        if (!cert.isExpired() && cert.daysUntilExpiry() <= daysThreshold && cert.daysUntilExpiry() >= 0) {
            expiring.append(cert);
        }
    }

    return expiring;
}

bool CertificationController::addCertification(const QString& engineerId, const QString& name,
                                              const QDate& dateEarned, const QDate& expiryDate)
{
    lastError_.clear();

    if (engineerId.isEmpty() || name.isEmpty() || !dateEarned.isValid()) {
        lastError_ = "Engineer ID, name, and date earned are required";
        return false;
    }

    Certification cert;
    cert.setEngineerId(engineerId);
    cert.setName(name);
    cert.setDateEarned(dateEarned);
    cert.setExpiryDate(expiryDate);

    CertificationRepository repo;
    bool success = repo.save(cert);
    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

bool CertificationController::updateCertification(int id, const QString& engineerId, const QString& name,
                                                 const QDate& dateEarned, const QDate& expiryDate)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Invalid certification ID";
        return false;
    }

    Certification cert;
    cert.setId(id);
    cert.setEngineerId(engineerId);
    cert.setName(name);
    cert.setDateEarned(dateEarned);
    cert.setExpiryDate(expiryDate);

    CertificationRepository repo;
    bool success = repo.save(cert);
    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

bool CertificationController::deleteCertification(int id)
{
    lastError_.clear();
    CertificationRepository repo;
    bool success = repo.remove(id);
    if (!success) {
        lastError_ = repo.lastError();
    }
    return success;
}
