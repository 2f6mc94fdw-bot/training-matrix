#include "Snapshot.h"

Snapshot::Snapshot()
    : id_(""), description_(""), timestamp_(QDateTime::currentDateTime()), data_(""), createdAt_(QDateTime::currentDateTime()) {}

Snapshot::~Snapshot() {}

bool Snapshot::isValid() const
{
    return !id_.isEmpty() && timestamp_.isValid();
}

QJsonObject Snapshot::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["description"] = description_;
    json["timestamp"] = timestamp_.toString(Qt::ISODate);
    json["data"] = data_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    return json;
}

Snapshot Snapshot::fromJson(const QJsonObject& json)
{
    Snapshot snapshot;
    snapshot.setId(json["id"].toString());
    snapshot.setDescription(json["description"].toString());
    snapshot.setTimestamp(QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate));
    snapshot.setData(json["data"].toString());
    snapshot.setCreatedAt(QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate));
    return snapshot;
}
