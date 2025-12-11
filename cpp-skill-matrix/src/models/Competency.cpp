#include "Competency.h"

Competency::Competency()
    : id_(0)
    , machineId_(0)
    , name_("")
    , maxScore_(3)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Competency::Competency(int id, int machineId, const QString& name, int maxScore)
    : id_(id)
    , machineId_(machineId)
    , name_(name)
    , maxScore_(maxScore)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Competency::~Competency()
{
}

bool Competency::isValid() const
{
    return id_ > 0 && machineId_ > 0 && !name_.isEmpty() && maxScore_ > 0;
}

QJsonObject Competency::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["machineId"] = machineId_;
    json["name"] = name_;
    json["maxScore"] = maxScore_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

Competency Competency::fromJson(const QJsonObject& json)
{
    Competency competency;
    competency.setId(json["id"].toInt());
    competency.setMachineId(json["machineId"].toInt());
    competency.setName(json["name"].toString());
    competency.setMaxScore(json["maxScore"].toInt(3));

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        competency.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        competency.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return competency;
}
