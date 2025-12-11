#ifndef AUDITLOGREPOSITORY_H
#define AUDITLOGREPOSITORY_H

#include "../models/AuditLog.h"
#include <QList>

class AuditLogRepository
{
public:
    AuditLogRepository();
    ~AuditLogRepository();

    QList<AuditLog> findAll(int limit = 1000);
    QList<AuditLog> findByUser(const QString& userId);
    bool save(AuditLog& log);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // AUDITLOGREPOSITORY_H
