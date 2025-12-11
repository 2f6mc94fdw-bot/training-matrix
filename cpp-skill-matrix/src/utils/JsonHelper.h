#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QVariantList>

/**
 * @brief Helper class for JSON operations
 */
class JsonHelper
{
public:
    /**
     * @brief Convert QVariantMap to JSON string
     */
    static QString toJsonString(const QVariantMap& map, bool indented = true);

    /**
     * @brief Convert QVariantList to JSON string
     */
    static QString toJsonString(const QVariantList& list, bool indented = true);

    /**
     * @brief Parse JSON string to QVariantMap
     */
    static QVariantMap parseObject(const QString& jsonString);

    /**
     * @brief Parse JSON string to QVariantList
     */
    static QVariantList parseArray(const QString& jsonString);

    /**
     * @brief Check if JSON string is valid
     */
    static bool isValid(const QString& jsonString);

    /**
     * @brief Get value from JSON object by path (e.g., "data.user.name")
     */
    static QVariant getValue(const QJsonObject& obj, const QString& path, const QVariant& defaultValue = QVariant());

    /**
     * @brief Set value in JSON object by path
     */
    static void setValue(QJsonObject& obj, const QString& path, const QVariant& value);

private:
    JsonHelper() = delete; // Static class
};

#endif // JSONHELPER_H
