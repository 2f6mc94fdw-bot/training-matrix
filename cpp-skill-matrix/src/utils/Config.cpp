#include "Config.h"
#include "Logger.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

Config& Config::instance()
{
    static Config instance;
    return instance;
}

Config::Config(QObject* parent)
    : QObject(parent)
{
}

Config::~Config()
{
}

bool Config::load(const QString& filePath)
{
    configFilePath_ = filePath.isEmpty() ? getDefaultConfigPath() : filePath;

    QFile file(configFilePath_);
    if (!file.exists()) {
        // Check if old "Skill Matrix" config exists and migrate it
        QString oldConfigPath = getOldSkillMatrixConfigPath();
        if (!oldConfigPath.isEmpty() && QFile::exists(oldConfigPath)) {
            Logger::instance().info("Config", "Migrating config from old Skill Matrix app: " + oldConfigPath);

            // Copy old config to new location
            QFile oldFile(oldConfigPath);
            if (oldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray data = oldFile.readAll();
                oldFile.close();

                // Ensure directory exists for new config
                QFileInfo fileInfo(configFilePath_);
                QDir dir = fileInfo.dir();
                if (!dir.exists()) {
                    dir.mkpath(".");
                }

                // Write to new location
                QFile newFile(configFilePath_);
                if (newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    newFile.write(data);
                    newFile.close();
                    Logger::instance().info("Config", "Successfully migrated config to: " + configFilePath_);
                }
            }
        } else {
            Logger::instance().warning("Config", "Config file not found: " + configFilePath_);
            // Create default config
            config_ = QVariantMap();
            return true; // Not an error - will use defaults
        }
    }

    // Now load the config (either existing or migrated)
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::instance().error("Config", "Failed to open config file: " + configFilePath_);
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        Logger::instance().error("Config", "Invalid JSON in config file");
        return false;
    }

    config_ = doc.object().toVariantMap();
    Logger::instance().info("Config", "Configuration loaded from: " + configFilePath_);
    return true;
}

bool Config::save(const QString& filePath)
{
    QString path = filePath.isEmpty() ? configFilePath_ : filePath;

    if (path.isEmpty()) {
        path = getDefaultConfigPath();
    }

    // Ensure directory exists
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::instance().error("Config", "Failed to save config file: " + path);
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromVariant(config_);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    configFilePath_ = path;
    Logger::instance().info("Config", "Configuration saved to: " + path);
    return true;
}

QVariant Config::get(const QString& key, const QVariant& defaultValue) const
{
    QStringList keys = parseKey(key);
    return getNestedValue(config_, keys, defaultValue);
}

void Config::set(const QString& key, const QVariant& value)
{
    QStringList keys = parseKey(key);
    setNestedValue(config_, keys, value);
    emit configChanged(key);
}

bool Config::has(const QString& key) const
{
    QStringList keys = parseKey(key);
    return !getNestedValue(config_, keys, QVariant()).isNull();
}

void Config::remove(const QString& key)
{
    // Simple implementation for top-level keys
    config_.remove(key);
    emit configChanged(key);
}

void Config::clear()
{
    config_.clear();
    emit configChanged("");
}

QString Config::databaseServer() const
{
    return get("database.server", "localhost").toString();
}

QString Config::databaseName() const
{
    return get("database.database", "training_matrix").toString();
}

QString Config::databaseUser() const
{
    return get("database.user", "sa").toString();
}

QString Config::databasePassword() const
{
    return get("database.password", "").toString();
}

int Config::databasePort() const
{
    return get("database.port", 1433).toInt();
}

bool Config::databaseEncrypt() const
{
    return get("database.encrypt", true).toBool();
}

bool Config::databaseTrustServerCertificate() const
{
    return get("database.trustServerCertificate", true).toBool();
}

void Config::setDatabaseServer(const QString& server)
{
    set("database.server", server);
}

void Config::setDatabaseName(const QString& name)
{
    set("database.database", name);
}

void Config::setDatabaseUser(const QString& user)
{
    set("database.user", user);
}

void Config::setDatabasePassword(const QString& password)
{
    set("database.password", password);
}

void Config::setDatabasePort(int port)
{
    set("database.port", port);
}

void Config::setDatabaseEncrypt(bool encrypt)
{
    set("database.encrypt", encrypt);
}

void Config::setDatabaseTrustServerCertificate(bool trust)
{
    set("database.trustServerCertificate", trust);
}

QString Config::getDefaultConfigPath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath("config.json");
}

QString Config::getOldSkillMatrixConfigPath() const
{
    // Get the path where the old "Skill Matrix" app stored its config
    // On macOS: ~/Library/Application Support/com.skillmatrix.app/config.json
    // On Windows: C:\Users\<user>\AppData\Local\com.skillmatrix.app\config.json
    // On Linux: ~/.local/share/com.skillmatrix.app/config.json

#ifdef Q_OS_MAC
    QString homePath = QDir::homePath();
    QString oldPath = homePath + "/Library/Application Support/com.skillmatrix.app/config.json";
    return oldPath;
#elif defined(Q_OS_WIN)
    QString homePath = QDir::homePath();
    QString oldPath = homePath + "/AppData/Local/com.skillmatrix.app/config.json";
    return oldPath;
#else
    QString homePath = QDir::homePath();
    QString oldPath = homePath + "/.local/share/com.skillmatrix.app/config.json";
    return oldPath;
#endif
}

QStringList Config::parseKey(const QString& key) const
{
    return key.split('.');
}

QVariant Config::getNestedValue(const QVariantMap& map, const QStringList& keys, const QVariant& defaultValue) const
{
    if (keys.isEmpty()) {
        return defaultValue;
    }

    QString key = keys.first();
    if (!map.contains(key)) {
        return defaultValue;
    }

    if (keys.size() == 1) {
        return map[key];
    }

    QVariant value = map[key];
    if (!value.canConvert<QVariantMap>()) {
        return defaultValue;
    }

    QVariantMap nestedMap = value.toMap();
    return getNestedValue(nestedMap, keys.mid(1), defaultValue);
}

void Config::setNestedValue(QVariantMap& map, const QStringList& keys, const QVariant& value)
{
    if (keys.isEmpty()) {
        return;
    }

    QString key = keys.first();

    if (keys.size() == 1) {
        map[key] = value;
        return;
    }

    if (!map.contains(key) || !map[key].canConvert<QVariantMap>()) {
        map[key] = QVariantMap();
    }

    QVariantMap nestedMap = map[key].toMap();
    setNestedValue(nestedMap, keys.mid(1), value);
    map[key] = nestedMap;
}
