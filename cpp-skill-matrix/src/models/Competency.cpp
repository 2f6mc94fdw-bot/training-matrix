#include "Competency.h"

Competency::Competency()
    : id_(0)
    , machineId_(0)
    , name_("")
    , maxScore_(3)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
    , safetyImpact_(3.0)
    , productionImpact_(3.0)
    , frequency_(3.0)
    , complexity_(3.0)
    , futureValue_(3.0)
{
}

Competency::Competency(int id, int machineId, const QString& name, int maxScore)
    : id_(id)
    , machineId_(machineId)
    , name_(name)
    , maxScore_(maxScore)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
    , safetyImpact_(3.0)
    , productionImpact_(3.0)
    , frequency_(3.0)
    , complexity_(3.0)
    , futureValue_(3.0)
{
}

Competency::~Competency()
{
}

double Competency::calculatedWeight() const
{
    // Multi-Criteria Weighted Scoring Formula
    // Safety: 30%, Production: 25%, Frequency: 20%, Complexity: 15%, Future: 10%
    return (safetyImpact_ * 0.30) +
           (productionImpact_ * 0.25) +
           (frequency_ * 0.20) +
           (complexity_ * 0.15) +
           (futureValue_ * 0.10);
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

    // Multi-Criteria Weighting
    json["safetyImpact"] = safetyImpact_;
    json["productionImpact"] = productionImpact_;
    json["frequency"] = frequency_;
    json["complexity"] = complexity_;
    json["futureValue"] = futureValue_;
    json["calculatedWeight"] = calculatedWeight();

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

    // Multi-Criteria Weighting (default to 3.0 if not present)
    competency.setSafetyImpact(json["safetyImpact"].toDouble(3.0));
    competency.setProductionImpact(json["productionImpact"].toDouble(3.0));
    competency.setFrequency(json["frequency"].toDouble(3.0));
    competency.setComplexity(json["complexity"].toDouble(3.0));
    competency.setFutureValue(json["futureValue"].toDouble(3.0));

    return competency;
}
