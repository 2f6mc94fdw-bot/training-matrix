#ifndef DATETIMEHELPER_H
#define DATETIMEHELPER_H

#include <QString>
#include <QDateTime>
#include <QDate>

/**
 * @brief Helper class for date/time operations
 */
class DateTimeHelper
{
public:
    /**
     * @brief Format datetime for display
     * @param dateTime DateTime to format
     * @return Formatted string
     */
    static QString formatDateTime(const QDateTime& dateTime);

    /**
     * @brief Format date for display
     * @param date Date to format
     * @return Formatted string
     */
    static QString formatDate(const QDate& date);

    /**
     * @brief Format time for display
     * @param time Time to format
     * @return Formatted string
     */
    static QString formatTime(const QTime& time);

    /**
     * @brief Parse datetime from string
     * @param str String to parse
     * @return QDateTime
     */
    static QDateTime parseDateTime(const QString& str);

    /**
     * @brief Parse date from string
     * @param str String to parse
     * @return QDate
     */
    static QDate parseDate(const QString& str);

    /**
     * @brief Get relative time string (e.g., "2 hours ago")
     * @param dateTime DateTime to compare
     * @return Relative time string
     */
    static QString relativeTime(const QDateTime& dateTime);

    /**
     * @brief Check if date is expired
     * @param expiryDate Expiry date to check
     * @return true if expired
     */
    static bool isExpired(const QDate& expiryDate);

    /**
     * @brief Get days until expiry
     * @param expiryDate Expiry date
     * @return Number of days (negative if expired)
     */
    static int daysUntilExpiry(const QDate& expiryDate);

    /**
     * @brief Format datetime for SQL
     * @param dateTime DateTime to format
     * @return SQL-formatted string
     */
    static QString toSqlDateTime(const QDateTime& dateTime);

    /**
     * @brief Format date for SQL
     * @param date Date to format
     * @return SQL-formatted string
     */
    static QString toSqlDate(const QDate& date);

private:
    DateTimeHelper() = delete; // Static class
};

#endif // DATETIMEHELPER_H
