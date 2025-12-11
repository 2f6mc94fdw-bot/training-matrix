#include "JsonHelper.h"

QString JsonHelper::toJsonString(const QVariantMap& map, bool indented)
{
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    return QString::fromUtf8(doc.toJson(indented ? QJsonDocument::Indented : QJsonDocument::Compact));
}

QString JsonHelper::toJsonString(const QVariantList& list, bool indented)
{
    QJsonDocument doc = QJsonDocument::fromVariant(list);
    return QString::fromUtf8(doc.toJson(indented ? QJsonDocument::Indented : QJsonDocument::Compact));
}

QVariantMap JsonHelper::parseObject(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return QVariantMap();
    }
    return doc.object().toVariantMap();
}

QVariantList JsonHelper::parseArray(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return QVariantList();
    }
    return doc.array().toVariantList();
}

bool JsonHelper::isValid(const QString& jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    return !doc.isNull();
}

QVariant JsonHelper::getValue(const QJsonObject& obj, const QString& path, const QVariant& defaultValue)
{
    QStringList keys = path.split('.');
    QJsonValue value = obj;

    for (const QString& key : keys) {
        if (!value.isObject()) {
            return defaultValue;
        }

        QJsonObject current = value.toObject();
        if (!current.contains(key)) {
            return defaultValue;
        }

        value = current[key];
    }

    return value.toVariant();
}

void JsonHelper::setValue(QJsonObject& obj, const QString& path, const QVariant& value)
{
    QStringList keys = path.split('.');
    if (keys.isEmpty()) {
        return;
    }

    QJsonObject* current = &obj;

    for (int i = 0; i < keys.size() - 1; ++i) {
        const QString& key = keys[i];

        if (!current->contains(key) || !(*current)[key].isObject()) {
            current->insert(key, QJsonObject());
        }

        QJsonValue val = (*current)[key];
        QJsonObject nested = val.toObject();
        current->insert(key, nested);
        current = &nested;
    }

    current->insert(keys.last(), QJsonValue::fromVariant(value));
}
