#include "Logger.h"
#include <QDir>
#include <QStandardPaths>
#include <iostream>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger(QObject* parent)
    : QObject(parent)
    , stream_(nullptr)
    , minLevel_(Info)
    , consoleOutput_(true)
    , initialized_(false)
{
}

Logger::~Logger()
{
    close();
}

bool Logger::initialize(const QString& logPath)
{
    if (initialized_) {
        return true;
    }

    QString path = logPath;

    // Auto-generate log path if not provided
    if (path.isEmpty()) {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd");
        path = dir.filePath(QString("skillmatrix_%1.log").arg(timestamp));
    }

    // Open log file
    logFile_.setFileName(path);
    if (!logFile_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        std::cerr << "Failed to open log file: " << path.toStdString() << std::endl;
        return false;
    }

    stream_ = new QTextStream(&logFile_);
    initialized_ = true;

    info("Logger", "Log file initialized: " + path);
    return true;
}

void Logger::close()
{
    if (!initialized_) {
        return;
    }

    if (stream_) {
        stream_->flush();
        delete stream_;
        stream_ = nullptr;
    }

    if (logFile_.isOpen()) {
        logFile_.close();
    }

    initialized_ = false;
}

void Logger::debug(const QString& category, const QString& message)
{
    log(Debug, category, message);
}

void Logger::info(const QString& category, const QString& message)
{
    log(Info, category, message);
}

void Logger::warning(const QString& category, const QString& message)
{
    log(Warning, category, message);
}

void Logger::error(const QString& category, const QString& message)
{
    log(Error, category, message);
}

void Logger::critical(const QString& category, const QString& message)
{
    log(Critical, category, message);
}

void Logger::log(LogLevel level, const QString& category, const QString& message)
{
    // Check if level is sufficient
    if (level < minLevel_) {
        return;
    }

    QMutexLocker locker(&mutex_);

    QString formattedMessage = formatMessage(level, category, message);

    // Write to file
    if (initialized_) {
        writeToFile(formattedMessage);
    }

    // Write to console
    if (consoleOutput_) {
        writeToConsole(formattedMessage);
    }

    // Emit signal
    emit messageLogged(level, category, message);
}

QString Logger::formatMessage(LogLevel level, const QString& category, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString levelStr = levelToString(level);

    return QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
        .arg(levelStr)
        .arg(category)
        .arg(message);
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
        case Debug:    return "DEBUG";
        case Info:     return "INFO ";
        case Warning:  return "WARN ";
        case Error:    return "ERROR";
        case Critical: return "CRIT ";
        default:       return "UNKN ";
    }
}

void Logger::writeToFile(const QString& formattedMessage)
{
    if (stream_) {
        *stream_ << formattedMessage << "\n";
        stream_->flush();
    }
}

void Logger::writeToConsole(const QString& formattedMessage)
{
    std::cout << formattedMessage.toStdString() << std::endl;
}
