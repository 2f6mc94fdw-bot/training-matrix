#include "AuditLog.h"

AuditLog::AuditLog()
    : id_(""), timestamp_(QDateTime::currentDateTime()), userId_(""), action_(""), details_(""), createdAt_(QDateTime::currentDateTime()) {}

AuditLog::~AuditLog() {}

bool AuditLog::isValid() const
{
    return !id_.isEmpty() && timestamp_.isValid() && !action_.isEmpty();
}

QJsonObject AuditLog::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["timestamp"] = timestamp_.toString(Qt::ISODate);
    json["userId"] = userId_;
    json["action"] = action_;
    json["details"] = details_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    return json;
}

AuditLog AuditLog::fromJson(const QJsonObject& json)
{
    AuditLog log;
    log.setId(json["id"].toString());
    log.setTimestamp(QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate));
    log.setUserId(json["userId"].toString());
    log.setAction(json["action"].toString());
    log.setDetails(json["details"].toString());
    log.setCreatedAt(QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate));
    return log;
}
