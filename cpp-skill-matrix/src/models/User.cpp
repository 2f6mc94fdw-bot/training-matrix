#include "User.h"
#include "../core/Constants.h"

User::User()
    : id_("")
    , username_("")
    , password_("")
    , role_(Constants::ROLE_ENGINEER)
    , engineerId_("")
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

User::User(const QString& id, const QString& username, const QString& password, const QString& role)
    : id_(id)
    , username_(username)
    , password_(password)
    , role_(role)
    , engineerId_("")
    , createdAt_(QDateTime::currentDateTime())
    , updatedAt_(QDateTime::currentDateTime())
{
}

User::~User()
{
}

bool User::isValid() const
{
    return !id_.isEmpty() && !username_.isEmpty() && !password_.isEmpty() && !role_.isEmpty();
}

bool User::isAdmin() const
{
    return role_ == Constants::ROLE_ADMIN;
}

bool User::isEngineer() const
{
    return role_ == Constants::ROLE_ENGINEER;
}

QJsonObject User::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["username"] = username_;
    json["role"] = role_;
    json["engineerId"] = engineerId_;
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    json["updatedAt"] = updatedAt_.toString(Qt::ISODate);
    // Note: Never serialize password to JSON
    return json;
}

User User::fromJson(const QJsonObject& json)
{
    User user;
    user.setId(json["id"].toString());
    user.setUsername(json["username"].toString());
    user.setRole(json["role"].toString());
    user.setEngineerId(json["engineerId"].toString());

    QString createdAtStr = json["createdAt"].toString();
    if (!createdAtStr.isEmpty()) {
        user.setCreatedAt(QDateTime::fromString(createdAtStr, Qt::ISODate));
    }

    QString updatedAtStr = json["updatedAt"].toString();
    if (!updatedAtStr.isEmpty()) {
        user.setUpdatedAt(QDateTime::fromString(updatedAtStr, Qt::ISODate));
    }

    return user;
}
