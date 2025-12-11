#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>

/**
 * @brief Database connection manager (Singleton)
 *
 * Manages SQL Server database connection and provides access to the database
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     */
    static DatabaseManager& instance();

    /**
     * @brief Connect to SQL Server database
     * @param server Server name or IP
     * @param database Database name
     * @param user Username
     * @param password Password
     * @param port Port number (default 1433)
     * @return true if connection successful
     */
    bool connect(const QString& server, const QString& database,
                const QString& user, const QString& password,
                int port = 1433);

    /**
     * @brief Disconnect from database
     */
    void disconnect();

    /**
     * @brief Test database connection
     * @return true if connected and can execute queries
     */
    bool testConnection();

    /**
     * @brief Get database connection
     * @return Reference to QSqlDatabase
     */
    QSqlDatabase& database() { return db_; }

    /**
     * @brief Check if connected to database
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Get last database error
     * @return Last error message
     */
    QString lastError() const;

    /**
     * @brief Execute SQL query (for utility/testing)
     * @param query SQL query string
     * @return true if successful
     */
    bool executeQuery(const QString& query);

    /**
     * @brief Begin transaction
     * @return true if successful
     */
    bool beginTransaction();

    /**
     * @brief Commit transaction
     * @return true if successful
     */
    bool commit();

    /**
     * @brief Rollback transaction
     * @return true if successful
     */
    bool rollback();

    /**
     * @brief Get connection string for debugging
     * @return Connection string (password masked)
     */
    QString connectionString() const;

signals:
    /**
     * @brief Emitted when connection status changes
     * @param connected true if connected
     */
    void connectionChanged(bool connected);

    /**
     * @brief Emitted when database error occurs
     * @param error Error message
     */
    void databaseError(const QString& error);

private:
    DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief Setup database connection parameters
     */
    void setupConnection(const QString& server, const QString& database,
                        const QString& user, const QString& password,
                        int port);

private:
    QSqlDatabase db_;
    QString server_;
    QString database_;
    QString user_;
    QString lastErrorMessage_;
    bool connected_;
};

#endif // DATABASEMANAGER_H
