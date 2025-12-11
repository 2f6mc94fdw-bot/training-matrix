#include "ProductionArea.h"

ProductionArea::ProductionArea()
    : id_(0)
    , name_("")
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

ProductionArea::ProductionArea(int id, const QString& name)
    : id_(id)
    , name_(name)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

ProductionArea::~ProductionArea()
{
}

bool ProductionArea::isValid() const
{
    return id_ > 0 && !name_.isEmpty();
}

QJsonObject ProductionArea::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["name"] = name_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

ProductionArea ProductionArea::fromJson(const QJsonObject& json)
{
    ProductionArea area;
    area.setId(json["id"].toInt());
    area.setName(json["name"].toString());

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        area.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        area.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return area;
}
