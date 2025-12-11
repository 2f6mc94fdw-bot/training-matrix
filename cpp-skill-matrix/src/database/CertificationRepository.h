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
    bool save(Certification& certification);
    bool remove(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // CERTIFICATIONREPOSITORY_H
