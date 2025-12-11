#ifndef CORESKILL_H
#define CORESKILL_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class CoreSkill
{
public:
    CoreSkill();
    ~CoreSkill();

    QString id() const { return id_; }
    QString categoryId() const { return categoryId_; }
    QString name() const { return name_; }
    int maxScore() const { return maxScore_; }
    QDateTime createdAt() const { return createdAt_; }

    void setId(const QString& id) { id_ = id; }
    void setCategoryId(const QString& categoryId) { categoryId_ = categoryId; }
    void setName(const QString& name) { name_ = name; }
    void setMaxScore(int maxScore) { maxScore_ = maxScore; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    bool isValid() const;
    QJsonObject toJson() const;
    static CoreSkill fromJson(const QJsonObject& json);

private:
    QString id_;
    QString categoryId_;
    QString name_;
    int maxScore_;
    QDateTime createdAt_;
};

#endif // CORESKILL_H
