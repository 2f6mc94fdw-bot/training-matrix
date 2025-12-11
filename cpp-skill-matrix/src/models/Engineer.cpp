#include "Engineer.h"

Engineer::Engineer()
    : id_("")
    , name_("")
    , shift_("")
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Engineer::Engineer(const QString& id, const QString& name, const QString& shift)
    : id_(id)
    , name_(name)
    , shift_(shift)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Engineer::~Engineer()
{
}

bool Engineer::isValid() const
{
    return !id_.isEmpty() && !name_.isEmpty() && !shift_.isEmpty();
}

QJsonObject Engineer::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["name"] = name_;
    json["shift"] = shift_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

Engineer Engineer::fromJson(const QJsonObject& json)
{
    Engineer engineer;
    engineer.setId(json["id"].toString());
    engineer.setName(json["name"].toString());
    engineer.setShift(json["shift"].toString());

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        engineer.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        engineer.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return engineer;
}
