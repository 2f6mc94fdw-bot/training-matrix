#include "CoreSkillCategory.h"

CoreSkillCategory::CoreSkillCategory()
    : id_(""), name_(""), createdAt_(QDateTime::currentDateTime())
{
}

CoreSkillCategory::CoreSkillCategory(const QString& id, const QString& name, const QString& discipline)
    : id_(id), name_(name), discipline_(discipline), createdAt_(QDateTime::currentDateTime())
{
}

CoreSkillCategory::~CoreSkillCategory()
{
}

bool CoreSkillCategory::isValid() const
{
    return !id_.isEmpty() && !name_.isEmpty();
}

QJsonObject CoreSkillCategory::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["name"] = name_;
    json["discipline"] = discipline_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    return json;
}

CoreSkillCategory CoreSkillCategory::fromJson(const QJsonObject& json)
{
    CoreSkillCategory category;
    category.setId(json["id"].toString());
    category.setName(json["name"].toString());
    category.setDiscipline(json["discipline"].toString());
    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        category.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }
    return category;
}
