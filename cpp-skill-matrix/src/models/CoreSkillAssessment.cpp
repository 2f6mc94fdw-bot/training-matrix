#include "CoreSkillAssessment.h"

CoreSkillAssessment::CoreSkillAssessment()
    : id_(0), engineerId_(""), categoryId_(""), skillId_(""), score_(0),
      createdAt_(QDateTime::currentDateTime()), updatedAt_(QDateTime::currentDateTime()) {}

CoreSkillAssessment::~CoreSkillAssessment() {}

bool CoreSkillAssessment::isValid() const
{
    return id_ > 0 && !engineerId_.isEmpty() && !categoryId_.isEmpty() && !skillId_.isEmpty();
}

QJsonObject CoreSkillAssessment::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["engineerId"] = engineerId_;
    json["categoryId"] = categoryId_;
    json["skillId"] = skillId_;
    json["score"] = score_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

CoreSkillAssessment CoreSkillAssessment::fromJson(const QJsonObject& json)
{
    CoreSkillAssessment assessment;
    assessment.setId(json["id"].toInt());
    assessment.setEngineerId(json["engineerId"].toString());
    assessment.setCategoryId(json["categoryId"].toString());
    assessment.setSkillId(json["skillId"].toString());
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
