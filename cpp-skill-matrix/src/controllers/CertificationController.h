#ifndef CERTIFICATIONCONTROLLER_H
#define CERTIFICATIONCONTROLLER_H

#include "../models/Certification.h"
#include <QList>
#include <QString>
#include <QDate>

/**
 * @brief Controller for Certification business logic
 */
class CertificationController
{
public:
    CertificationController();
    ~CertificationController();

    QList<Certification> getAllCertifications();
    QList<Certification> getCertificationsByEngineer(const QString& engineerId);
    QList<Certification> getExpiredCertifications();
    QList<Certification> getExpiringCertifications(int daysThreshold = 30);
    bool addCertification(const QString& engineerId, const QString& name,
                         const QDate& dateEarned, const QDate& expiryDate);
    bool updateCertification(int id, const QString& engineerId, const QString& name,
                            const QDate& dateEarned, const QDate& expiryDate);
    bool deleteCertification(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // CERTIFICATIONCONTROLLER_H
