#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>

/**
 * @brief Logger class for application-wide logging
 *
 * Singleton logger that writes to file and optionally to console
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };

    /**
     * @brief Get singleton instance
     */
    static Logger& instance();

    /**
     * @brief Initialize logger
     * @param logPath Path to log file (optional, auto-generated if empty)
     * @return true if successful
     */
    bool initialize(const QString& logPath = QString());

    /**
     * @brief Close logger
     */
    void close();

    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    void setLevel(LogLevel level) { minLevel_ = level; }

    /**
     * @brief Get current log level
     */
    LogLevel level() const { return minLevel_; }

    /**
     * @brief Enable/disable console output
     */
    void setConsoleOutput(bool enabled) { consoleOutput_ = enabled; }

    /**
     * @brief Log debug message
     */
    void debug(const QString& category, const QString& message);

    /**
     * @brief Log info message
     */
    void info(const QString& category, const QString& message);

    /**
     * @brief Log warning message
     */
    void warning(const QString& category, const QString& message);

    /**
     * @brief Log error message
     */
    void error(const QString& category, const QString& message);

    /**
     * @brief Log critical message
     */
    void critical(const QString& category, const QString& message);

    /**
     * @brief Log message with custom level
     */
    void log(LogLevel level, const QString& category, const QString& message);

signals:
    /**
     * @brief Emitted when a message is logged
     */
    void messageLogged(Logger::LogLevel level, const QString& category, const QString& message);

private:
    Logger(QObject* parent = nullptr);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Format log message
     */
    QString formatMessage(LogLevel level, const QString& category, const QString& message);

    /**
     * @brief Get level string
     */
    QString levelToString(LogLevel level) const;

    /**
     * @brief Write message to file
     */
    void writeToFile(const QString& formattedMessage);

    /**
     * @brief Write message to console
     */
    void writeToConsole(const QString& formattedMessage);

private:
    QFile logFile_;
    QTextStream* stream_;
    QMutex mutex_;
    LogLevel minLevel_;
    bool consoleOutput_;
    bool initialized_;
};

#endif // LOGGER_H
