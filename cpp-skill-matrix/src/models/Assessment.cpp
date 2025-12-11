#include "Assessment.h"

Assessment::Assessment()
    : id_(0)
    , engineerId_("")
    , productionAreaId_(0)
    , machineId_(0)
    , competencyId_(0)
    , score_(0)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Assessment::Assessment(int id, const QString& engineerId, int productionAreaId,
                      int machineId, int competencyId, int score)
    : id_(id)
    , engineerId_(engineerId)
    , productionAreaId_(productionAreaId)
    , machineId_(machineId)
    , competencyId_(competencyId)
    , score_(score)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Assessment::~Assessment()
{
}

bool Assessment::isValid() const
{
    return id_ > 0 && !engineerId_.isEmpty() &&
           productionAreaId_ > 0 && machineId_ > 0 &&
           competencyId_ > 0 && score_ >= 0;
}

QJsonObject Assessment::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["engineerId"] = engineerId_;
    json["productionAreaId"] = productionAreaId_;
    json["machineId"] = machineId_;
    json["competencyId"] = competencyId_;
    json["score"] = score_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

Assessment Assessment::fromJson(const QJsonObject& json)
{
    Assessment assessment;
    assessment.setId(json["id"].toInt());
    assessment.setEngineerId(json["engineerId"].toString());
    assessment.setProductionAreaId(json["productionAreaId"].toInt());
    assessment.setMachineId(json["machineId"].toInt());
    assessment.setCompetencyId(json["competencyId"].toInt());
    assessment.setScore(json["score"].toInt());

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        assessment.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        assessment.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return assessment;
}

QString Assessment::getKey() const
{
    return makeKey(engineerId_, productionAreaId_, machineId_, competencyId_);
}

QString Assessment::makeKey(const QString& engineerId, int productionAreaId,
                           int machineId, int competencyId)
{
    return QString("%1-%2-%3-%4")
        .arg(engineerId)
        .arg(productionAreaId)
        .arg(machineId)
        .arg(competencyId);
}
