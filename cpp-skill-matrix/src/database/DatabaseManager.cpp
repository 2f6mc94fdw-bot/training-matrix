#include "DatabaseManager.h"
#include "../core/Constants.h"
#include "../utils/Logger.h"

#include <QSqlQuery>
#include <QSqlDriver>
#include <QElapsedTimer>

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
                              int port,
                              bool encrypt,
                              bool trustServerCertificate)
{
    QElapsedTimer timer;
    timer.start();
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
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=%1,%2;"
        "DATABASE=%3;"
        "UID=%4;"
        "PWD=%5;"
        "Encrypt=%6;"
        "TrustServerCertificate=%7;"
        "LoginTimeout=%8;"
        "Connection Timeout=%8;"
    ).arg(server)
     .arg(port)
     .arg(database)
     .arg(user)
     .arg(password)
     .arg(encrypt ? "yes" : "no")
     .arg(trustServerCertificate ? "yes" : "no")
     .arg(Constants::DB_CONNECTION_TIMEOUT / 1000);

    db_.setConnectOptions(QString("SQL_ATTR_LOGIN_TIMEOUT=%1;SQL_ATTR_CONNECTION_TIMEOUT=%1")
        .arg(Constants::DB_CONNECTION_TIMEOUT / 1000));

    db_.setDatabaseName(connectionString);

    // Open connection
    if (!db_.open()) {
        lastErrorMessage_ = db_.lastError().text();
        Logger::instance().error("DatabaseManager", "Failed to connect: " + lastErrorMessage_);
        Logger::instance().warning("DatabaseManager", QString("Connection attempt duration: %1 ms").arg(timer.elapsed()));
        emit databaseError(lastErrorMessage_);
        connected_ = false;
        emit connectionChanged(false);
        return false;
    }

    // Test connection
    if (!testConnection()) {
        lastErrorMessage_ = "Connection opened but failed to execute test query";
        Logger::instance().error("DatabaseManager", lastErrorMessage_);
        Logger::instance().warning("DatabaseManager", QString("Connection attempt duration: %1 ms").arg(timer.elapsed()));
        db_.close();
        emit databaseError(lastErrorMessage_);
        connected_ = false;
        emit connectionChanged(false);
        return false;
    }

    if (!verifyRequiredSchema()) {
        Logger::instance().error("DatabaseManager", lastErrorMessage_);
        Logger::instance().warning("DatabaseManager", QString("Schema validation duration: %1 ms").arg(timer.elapsed()));
        db_.close();
        emit databaseError(lastErrorMessage_);
        connected_ = false;
        emit connectionChanged(false);
        return false;
    }

    connected_ = true;
    emit connectionChanged(true);

    Logger::instance().info("DatabaseManager", QString("Successfully connected to database in %1 ms").arg(timer.elapsed()));
    return true;
}

void DatabaseManager::disconnect()
{
    // During static destruction, the Qt database driver may already be destroyed
    // Use QSqlDatabase::contains() which is a static method that doesn't access
    // the database object itself, preventing crashes during shutdown
    if (QSqlDatabase::contains(Constants::DB_CONNECTION_NAME)) {
        if (db_.isOpen()) {
            db_.close();
            Logger::instance().info("DatabaseManager", "Disconnected from database");
        }
    }

    if (connected_) {
        connected_ = false;
        emit connectionChanged(false);
    }
}

bool DatabaseManager::testConnection()
{
    if (!db_.isValid() || !db_.isOpen()) {
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

bool DatabaseManager::verifyRequiredSchema()
{
    if (!db_.isValid() || !db_.isOpen()) {
        lastErrorMessage_ = "Database connection is not open";
        return false;
    }

    QSqlQuery query(db_);
    const QString validationSql = QStringLiteral(R"SQL(
        SELECT
            CASE WHEN
                OBJECT_ID(N'dbo.users', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.engineers', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.production_areas', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.machines', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.competencies', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.assessments', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.core_skill_categories', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.core_skills', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.core_skill_assessments', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.certifications', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.notifications', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.development_plan_items', N'U') IS NOT NULL AND
                OBJECT_ID(N'dbo.assessment_submissions', N'U') IS NOT NULL AND
                COL_LENGTH('dbo.competencies', 'safety_impact') IS NOT NULL AND
                COL_LENGTH('dbo.competencies', 'production_impact') IS NOT NULL AND
                COL_LENGTH('dbo.competencies', 'frequency') IS NOT NULL AND
                COL_LENGTH('dbo.competencies', 'complexity') IS NOT NULL AND
                COL_LENGTH('dbo.competencies', 'future_value') IS NOT NULL AND
                COL_LENGTH('dbo.core_skills', 'safety_impact') IS NOT NULL AND
                COL_LENGTH('dbo.core_skills', 'production_impact') IS NOT NULL AND
                COL_LENGTH('dbo.core_skills', 'frequency') IS NOT NULL AND
                COL_LENGTH('dbo.core_skills', 'complexity') IS NOT NULL AND
                COL_LENGTH('dbo.core_skills', 'future_value') IS NOT NULL AND
                COL_LENGTH('dbo.core_skill_categories', 'discipline') IS NOT NULL AND
                COL_LENGTH('dbo.certifications', 'certificate_file_path') IS NOT NULL AND
                COL_LENGTH('dbo.assessment_submissions', 'row_version') IS NOT NULL
            THEN 1 ELSE 0 END AS [schema_ok],
            CASE WHEN OBJECT_ID(N'dbo.app_schema_versions', N'U') IS NOT NULL
            THEN 1 ELSE 0 END AS [has_version_ledger]
    )SQL");

    if (!query.exec(validationSql) || !query.next()) {
        lastErrorMessage_ = "Unable to validate the Aptitude database schema: " + query.lastError().text();
        return false;
    }

    if (query.value(0).toInt() != 1) {
        lastErrorMessage_ =
            "The Aptitude database schema is incomplete or outdated. "
            "Ask an administrator to run schema.sql and PRODUCTION_SCHEMA_MIGRATION.sql.";
        return false;
    }

    if (query.value(1).toInt() == 1) {
        QSqlQuery versionQuery(db_);
        if (!versionQuery.exec("SELECT MAX([version]) FROM [dbo].[app_schema_versions]") ||
            !versionQuery.next() || versionQuery.value(0).toLongLong() < 2026072701LL) {
            lastErrorMessage_ =
                "The Aptitude database schema version is older than this application. "
                "Ask an administrator to run PRODUCTION_SCHEMA_MIGRATION.sql.";
            return false;
        }
    } else {
        Logger::instance().warning("DatabaseManager",
            "Connected to a legacy database without a schema version ledger; migrate before the Windows pilot");
    }

    return true;
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
    // SECURITY WARNING: This method is deprecated and should not be used for queries with user input
    Logger::instance().warning("DatabaseManager",
        "executeQuery() is deprecated - use prepared statements with parameter binding instead");

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
