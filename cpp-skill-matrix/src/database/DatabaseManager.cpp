#include "DatabaseManager.h"
#include "../core/Constants.h"
#include "../utils/Logger.h"

#include <QSqlQuery>
#include <QSqlDriver>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
    , connected_(false)
{
    // Initialize database connection
    db_ = QSqlDatabase::addDatabase("QODBC", Constants::DB_CONNECTION_NAME);
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::connect(const QString& server, const QString& database,
                              const QString& user, const QString& password,
                              int port)
{
    Logger::instance().info("DatabaseManager", QString("Connecting to SQL Server: %1/%2").arg(server).arg(database));

    // Disconnect if already connected
    if (connected_) {
        disconnect();
    }

    // Store connection parameters
    server_ = server;
    database_ = database;
    user_ = user;

    // Build connection string for SQL Server
    QString connectionString = QString(
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=%1,%2;"
        "DATABASE=%3;"
        "UID=%4;"
        "PWD=%5;"
        "Encrypt=yes;"
        "TrustServerCertificate=yes;"
    ).arg(server).arg(port).arg(database).arg(user).arg(password);

    // Try alternate driver if 17 is not available
    // Note: Users may have different ODBC driver versions installed
    // Common versions: ODBC Driver 17/18 for SQL Server, SQL Server Native Client 11.0

    db_.setDatabaseName(connectionString);

    // Open connection
    if (!db_.open()) {
        lastErrorMessage_ = db_.lastError().text();
        Logger::instance().error("DatabaseManager", "Failed to connect: " + lastErrorMessage_);
        emit databaseError(lastErrorMessage_);
        connected_ = false;
        emit connectionChanged(false);
        return false;
    }

    // Test connection
    if (!testConnection()) {
        lastErrorMessage_ = "Connection opened but failed to execute test query";
        Logger::instance().error("DatabaseManager", lastErrorMessage_);
        db_.close();
        emit databaseError(lastErrorMessage_);
        connected_ = false;
        emit connectionChanged(false);
        return false;
    }

    connected_ = true;
    emit connectionChanged(true);
    Logger::instance().info("DatabaseManager", "Successfully connected to database");
    return true;
}

void DatabaseManager::disconnect()
{
    if (db_.isOpen()) {
        db_.close();
        Logger::instance().info("DatabaseManager", "Disconnected from database");
    }

    if (connected_) {
        connected_ = false;
        emit connectionChanged(false);
    }
}

bool DatabaseManager::testConnection()
{
    if (!db_.isOpen()) {
        return false;
    }

    QSqlQuery query(db_);
    bool success = query.exec("SELECT 1");

    if (!success) {
        lastErrorMessage_ = query.lastError().text();
        Logger::instance().error("DatabaseManager", "Test query failed: " + lastErrorMessage_);
    }

    return success;
}

bool DatabaseManager::isConnected() const
{
    return connected_ && db_.isOpen();
}

QString DatabaseManager::lastError() const
{
    return lastErrorMessage_;
}

bool DatabaseManager::executeQuery(const QString& query)
{
    if (!isConnected()) {
        lastErrorMessage_ = "Not connected to database";
        return false;
    }

    QSqlQuery sqlQuery(db_);
    if (!sqlQuery.exec(query)) {
        lastErrorMessage_ = sqlQuery.lastError().text();
        Logger::instance().error("DatabaseManager", "Query failed: " + lastErrorMessage_);
        emit databaseError(lastErrorMessage_);
        return false;
    }

    return true;
}

bool DatabaseManager::beginTransaction()
{
    if (!isConnected()) {
        lastErrorMessage_ = "Not connected to database";
        return false;
    }

    if (!db_.transaction()) {
        lastErrorMessage_ = db_.lastError().text();
        Logger::instance().error("DatabaseManager", "Failed to begin transaction: " + lastErrorMessage_);
        return false;
    }

    Logger::instance().debug("DatabaseManager", "Transaction started");
    return true;
}

bool DatabaseManager::commit()
{
    if (!isConnected()) {
        lastErrorMessage_ = "Not connected to database";
        return false;
    }

    if (!db_.commit()) {
        lastErrorMessage_ = db_.lastError().text();
        Logger::instance().error("DatabaseManager", "Failed to commit transaction: " + lastErrorMessage_);
        return false;
    }

    Logger::instance().debug("DatabaseManager", "Transaction committed");
    return true;
}

bool DatabaseManager::rollback()
{
    if (!isConnected()) {
        lastErrorMessage_ = "Not connected to database";
        return false;
    }

    if (!db_.rollback()) {
        lastErrorMessage_ = db_.lastError().text();
        Logger::instance().error("DatabaseManager", "Failed to rollback transaction: " + lastErrorMessage_);
        return false;
    }

    Logger::instance().debug("DatabaseManager", "Transaction rolled back");
    return true;
}

QString DatabaseManager::connectionString() const
{
    return QString("Server=%1, Database=%2, User=%3").arg(server_).arg(database_).arg(user_);
}
