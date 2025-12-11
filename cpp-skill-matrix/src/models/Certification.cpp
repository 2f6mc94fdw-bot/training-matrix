#include "Certification.h"

Certification::Certification()
    : id_(0), engineerId_(""), name_(""), dateEarned_(), expiryDate_(), createdAt_(QDateTime::currentDateTime()) {}

Certification::~Certification() {}

bool Certification::isValid() const
{
    return id_ > 0 && !engineerId_.isEmpty() && !name_.isEmpty() && dateEarned_.isValid();
}

bool Certification::isExpired() const
{
    return expiryDate_.isValid() && expiryDate_ < QDate::currentDate();
}

int Certification::daysUntilExpiry() const
{
    return expiryDate_.isValid() ? QDate::currentDate().daysTo(expiryDate_) : 0;
}

QJsonObject Certification::toJson() const
{
    QJsonObject json;
    json["id"] = id_;
    json["engineerId"] = engineerId_;
    json["name"] = name_;
    json["dateEarned"] = dateEarned_.toString(Qt::ISODate);
    json["expiryDate"] = expiryDate_.toString(Qt::ISODate);
    json["createdAt"] = createdAt_.toString(Qt::ISODate);
    return json;
}

Certification Certification::fromJson(const QJsonObject& json)
{
    Certification cert;
    cert.setId(json["id"].toInt());
    cert.setEngineerId(json["engineerId"].toString());
    cert.setName(json["name"].toString());
    cert.setDateEarned(QDate::fromString(json["dateEarned"].toString(), Qt::ISODate));
    cert.setExpiryDate(QDate::fromString(json["expiryDate"].toString(), Qt::ISODate));
    cert.setCreatedAt(QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate));
    return cert;
}
