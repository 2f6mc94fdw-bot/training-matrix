#include "DateTimeHelper.h"

QString DateTimeHelper::formatDateTime(const QDateTime& dateTime)
{
    if (!dateTime.isValid()) {
        return QString();
    }
    return dateTime.toString("yyyy-MM-dd HH:mm:ss");
}

QString DateTimeHelper::formatDate(const QDate& date)
{
    if (!date.isValid()) {
        return QString();
    }
    return date.toString("yyyy-MM-dd");
}

QString DateTimeHelper::formatTime(const QTime& time)
{
    if (!time.isValid()) {
        return QString();
    }
    return time.toString("HH:mm:ss");
}

QDateTime DateTimeHelper::parseDateTime(const QString& str)
{
    // Try multiple formats
    QDateTime dt = QDateTime::fromString(str, "yyyy-MM-dd HH:mm:ss");
    if (!dt.isValid()) {
        dt = QDateTime::fromString(str, Qt::ISODate);
    }
    return dt;
}

QDate DateTimeHelper::parseDate(const QString& str)
{
    QDate date = QDate::fromString(str, "yyyy-MM-dd");
    if (!date.isValid()) {
        date = QDate::fromString(str, Qt::ISODate);
    }
    return date;
}

QString DateTimeHelper::relativeTime(const QDateTime& dateTime)
{
    if (!dateTime.isValid()) {
        return "Never";
    }

    qint64 seconds = dateTime.secsTo(QDateTime::currentDateTime());

    if (seconds < 60) {
        return "Just now";
    } else if (seconds < 3600) {
        int minutes = seconds / 60;
        return QString("%1 minute%2 ago").arg(minutes).arg(minutes > 1 ? "s" : "");
    } else if (seconds < 86400) {
        int hours = seconds / 3600;
        return QString("%1 hour%2 ago").arg(hours).arg(hours > 1 ? "s" : "");
    } else if (seconds < 604800) {
        int days = seconds / 86400;
        return QString("%1 day%2 ago").arg(days).arg(days > 1 ? "s" : "");
    } else {
        return formatDate(dateTime.date());
    }
}

bool DateTimeHelper::isExpired(const QDate& expiryDate)
{
    if (!expiryDate.isValid()) {
        return false;
    }
    return expiryDate < QDate::currentDate();
}

int DateTimeHelper::daysUntilExpiry(const QDate& expiryDate)
{
    if (!expiryDate.isValid()) {
        return 0;
    }
    return QDate::currentDate().daysTo(expiryDate);
}

QString DateTimeHelper::toSqlDateTime(const QDateTime& dateTime)
{
    if (!dateTime.isValid()) {
        return "NULL";
    }
    return dateTime.toString("yyyy-MM-dd HH:mm:ss");
}

QString DateTimeHelper::toSqlDate(const QDate& date)
{
    if (!date.isValid()) {
        return "NULL";
    }
    return date.toString("yyyy-MM-dd");
}
