#ifndef CORESKILLCATEGORY_H
#define CORESKILLCATEGORY_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class CoreSkillCategory
{
public:
    CoreSkillCategory();
    CoreSkillCategory(const QString& id, const QString& name);
    ~CoreSkillCategory();

    QString id() const { return id_; }
    QString name() const { return name_; }
    QDateTime createdAt() const { return createdAt_; }

    void setId(const QString& id) { id_ = id; }
    void setName(const QString& name) { name_ = name; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    bool isValid() const;
    QJsonObject toJson() const;
    static CoreSkillCategory fromJson(const QJsonObject& json);

    bool operator==(const CoreSkillCategory& other) const { return id_ == other.id_; }

private:
    QString id_;
    QString name_;
    QDateTime createdAt_;
};

#endif // CORESKILLCATEGORY_H
