#ifndef NOTIFICATIONREPOSITORY_H
#define NOTIFICATIONREPOSITORY_H

#include <QDateTime>
#include <QList>
#include <QString>

struct EngineerNotification {
    QString id;
    QString engineerId;
    QString title;
    QString message;
    bool isRead = false;
    QDateTime createdAt;
    QDateTime readAt;
    QString createdByUserId;
    QString createdByName;
};

class NotificationRepository
{
public:
    NotificationRepository();
    ~NotificationRepository();

    bool createForEngineer(const QString& engineerId,
                           const QString& title,
                           const QString& message,
                           const QString& createdByUserId,
                           const QString& createdByName);
    bool createForRole(const QString& role,
                       const QString& title,
                       const QString& message,
                       const QString& createdByUserId,
                       const QString& createdByName);

    QList<EngineerNotification> findByEngineer(const QString& engineerId,
                                               bool onlyUnread = false,
                                               int limit = 50);
    QList<EngineerNotification> findByRecipientUser(const QString& userId,
                                                    bool onlyUnread = false,
                                                    int limit = 100);

    bool markAllReadForEngineer(const QString& engineerId);
    bool markAllReadForUser(const QString& userId);

    QString lastError() const { return lastError_; }

private:
    bool ensureSchema();

private:
    QString lastError_;
    bool schemaEnsured_ = false;
};

#endif // NOTIFICATIONREPOSITORY_H
