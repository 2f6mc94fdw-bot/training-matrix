#ifndef AUDITLOG_H
#define AUDITLOG_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class AuditLog
{
public:
    AuditLog();
    ~AuditLog();

    QString id() const { return id_; }
    QDateTime timestamp() const { return timestamp_; }
    QString userId() const { return userId_; }
    QString action() const { return action_; }
    QString details() const { return details_; }
    QDateTime createdAt() const { return createdAt_; }

    void setId(const QString& id) { id_ = id; }
    void setTimestamp(const QDateTime& timestamp) { timestamp_ = timestamp; }
    void setUserId(const QString& userId) { userId_ = userId; }
    void setAction(const QString& action) { action_ = action; }
    void setDetails(const QString& details) { details_ = details; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    bool isValid() const;
    QJsonObject toJson() const;
    static AuditLog fromJson(const QJsonObject& json);

private:
    QString id_;
    QDateTime timestamp_;
    QString userId_;
    QString action_;
    QString details_;
    QDateTime createdAt_;
};

#endif // AUDITLOG_H
