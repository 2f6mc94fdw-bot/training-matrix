#include "CoreSkill.h"

CoreSkill::CoreSkill() : id_(""), categoryId_(""), name_(""), maxScore_(3), createdAt_(QDateTime::currentDateTime()) {}
CoreSkill::~CoreSkill() {}

bool CoreSkill::isValid() const { return !id_.isEmpty() && !categoryId_.isEmpty() && !name_.isEmpty(); }

QJsonObject CoreSkill::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["categoryId"] = categoryId_;
    json["name"] = name_;
    json["maxScore"] = maxScore_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
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
    return skill;
}
