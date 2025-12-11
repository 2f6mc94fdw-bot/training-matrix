#include "CertificationRepository.h"

CertificationRepository::CertificationRepository() : lastError_("") {}
CertificationRepository::~CertificationRepository() {}

QList<Certification> CertificationRepository::findAll() { return QList<Certification>(); }
QList<Certification> CertificationRepository::findByEngineer(const QString& engineerId) { return QList<Certification>(); }
bool CertificationRepository::save(Certification& certification) { return false; }
bool CertificationRepository::remove(int id) { return false; }
