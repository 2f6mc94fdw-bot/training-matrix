#include "Machine.h"

Machine::Machine()
    : id_(0)
    , productionAreaId_(0)
    , name_("")
    , importance_(0)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Machine::Machine(int id, int productionAreaId, const QString& name, int importance)
    : id_(id)
    , productionAreaId_(productionAreaId)
    , name_(name)
    , importance_(importance)
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

Machine::~Machine()
{
}

bool Machine::isValid() const
{
    return id_ > 0 && productionAreaId_ > 0 && !name_.isEmpty() &&
           importance_ >= 0 && importance_ <= 3;
}

QJsonObject Machine::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["productionAreaId"] = productionAreaId_;
    json["name"] = name_;
    json["importance"] = importance_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    return json;
}

Machine Machine::fromJson(const QJsonObject& json)
{
    Machine machine;
    machine.setId(json["id"].toInt());
    machine.setProductionAreaId(json["productionAreaId"].toInt());
    machine.setName(json["name"].toString());
    machine.setImportance(json["importance"].toInt(0));

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        machine.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        machine.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return machine;
}
