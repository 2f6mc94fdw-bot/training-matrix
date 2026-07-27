#ifndef CERTIFICATIONREPOSITORY_H
#define CERTIFICATIONREPOSITORY_H

#include "../models/Certification.h"
#include <QList>

class CertificationRepository
{
public:
    CertificationRepository();
    ~CertificationRepository();

    QList<Certification> findAll();
    QList<Certification> findByEngineer(const QString& engineerId);
    Certification findById(int id);
    bool save(Certification& certification);
    bool updateCertificateFilePath(int id, const QString& filePath);
    bool remove(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // CERTIFICATIONREPOSITORY_H
