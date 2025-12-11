#include "AuditLogRepository.h"

AuditLogRepository::AuditLogRepository() : lastError_("") {}
AuditLogRepository::~AuditLogRepository() {}

QList<AuditLog> AuditLogRepository::findAll(int limit) { return QList<AuditLog>(); }
QList<AuditLog> AuditLogRepository::findByUser(const QString& userId) { return QList<AuditLog>(); }
bool AuditLogRepository::save(AuditLog& log) { return false; }
