#include "CoreSkill.h"

CoreSkill::CoreSkill()
    : id_("")
    , categoryId_("")
    , name_("")
    , maxScore_(3)
    , createdAt_(QDateTime::currentDateTime())
    , safetyImpact_(3.0)
    , productionImpact_(3.0)
    , frequency_(3.0)
    , complexity_(3.0)
    , futureValue_(3.0)
{
}

CoreSkill::~CoreSkill() {}

double CoreSkill::calculatedWeight() const
{
    // Multi-Criteria Weighted Scoring Formula
    // Safety: 30%, Production: 25%, Frequency: 20%, Complexity: 15%, Future: 10%
    return (safetyImpact_ * 0.30) +
           (productionImpact_ * 0.25) +
           (frequency_ * 0.20) +
           (complexity_ * 0.15) +
           (futureValue_ * 0.10);
}

bool CoreSkill::isValid() const { return !id_.isEmpty() && !categoryId_.isEmpty() && !name_.isEmpty(); }

QJsonObject CoreSkill::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["categoryId"] = categoryId_;
    json["name"] = name_;
    json["maxScore"] = maxScore_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);

    // Multi-Criteria Weighting
    json["safetyImpact"] = safetyImpact_;
    json["productionImpact"] = productionImpact_;
    json["frequency"] = frequency_;
    json["complexity"] = complexity_;
    json["futureValue"] = futureValue_;
    json["calculatedWeight"] = calculatedWeight();

    return json;
}

CoreSkill CoreSkill::fromJson(const QJsonObject& json)
{
    CoreSkill skill;
    skill.setId(json["id"].toString());
    skill.setCategoryId(json["categoryId"].toString());
    skill.setName(json["name"].toString());
    skill.setMaxScore(json["maxScore"].toInt(3));

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        skill.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    // Multi-Criteria Weighting (default to 3.0 if not present)
    skill.setSafetyImpact(json["safetyImpact"].toDouble(3.0));
    skill.setProductionImpact(json["productionImpact"].toDouble(3.0));
    skill.setFrequency(json["frequency"].toDouble(3.0));
    skill.setComplexity(json["complexity"].toDouble(3.0));
    skill.setFutureValue(json["futureValue"].toDouble(3.0));

    return skill;
}
