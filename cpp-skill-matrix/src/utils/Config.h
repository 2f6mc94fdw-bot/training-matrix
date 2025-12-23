#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>

/**
 * @brief Configuration manager for database connection and app settings
 *
 * Manages application configuration stored in JSON format
 */
class Config : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     */
    static Config& instance();

    /**
     * @brief Load configuration from file
     * @param filePath Path to config file (optional, uses default if empty)
     * @return true if successful
     */
    bool load(const QString& filePath = QString());

    /**
     * @brief Save configuration to file
     * @param filePath Path to config file (optional, uses default if empty)
     * @return true if successful
     */
    bool save(const QString& filePath = QString());

    /**
     * @brief Get configuration value
     * @param key Configuration key (supports dot notation, e.g., "database.server")
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value
     */
    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set configuration value
     * @param key Configuration key (supports dot notation)
     * @param value Value to set
     */
    void set(const QString& key, const QVariant& value);

    /**
     * @brief Check if key exists
     * @param key Configuration key
     * @return true if key exists
     */
    bool has(const QString& key) const;

    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     */
    void remove(const QString& key);

    /**
     * @brief Clear all configuration
     */
    void clear();

    // Database configuration helpers
    QString databaseServer() const;
    QString databaseName() const;
    QString databaseUser() const;
    QString databasePassword() const;
    int databasePort() const;
    bool databaseEncrypt() const;
    bool databaseTrustServerCertificate() const;

    void setDatabaseServer(const QString& server);
    void setDatabaseName(const QString& name);
    void setDatabaseUser(const QString& user);
    void setDatabasePassword(const QString& password);
    void setDatabasePort(int port);
    void setDatabaseEncrypt(bool encrypt);
    void setDatabaseTrustServerCertificate(bool trust);

signals:
    /**
     * @brief Emitted when configuration changes
     * @param key Changed key
     */
    void configChanged(const QString& key);

private:
    Config(QObject* parent = nullptr);
    ~Config();

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    /**
     * @brief Get default config file path
     */
    QString getDefaultConfigPath() const;

    /**
     * @brief Get old Skill Matrix config file path for migration
     */
    QString getOldSkillMatrixConfigPath() const;

    /**
     * @brief Parse nested key (e.g., "database.server" -> ["database", "server"])
     */
    QStringList parseKey(const QString& key) const;

    /**
     * @brief Get nested value from map
     */
    QVariant getNestedValue(const QVariantMap& map, const QStringList& keys, const QVariant& defaultValue) const;

    /**
     * @brief Set nested value in map
     */
    void setNestedValue(QVariantMap& map, const QStringList& keys, const QVariant& value);

private:
    QVariantMap config_;
    QString configFilePath_;
};

#endif // CONFIG_H
