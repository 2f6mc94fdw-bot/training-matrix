#ifndef CORESKILLASSESSMENT_H
#define CORESKILLASSESSMENT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class CoreSkillAssessment
{
public:
    CoreSkillAssessment();
    ~CoreSkillAssessment();

    int id() const { return id_; }
    QString engineerId() const { return engineerId_; }
    QString categoryId() const { return categoryId_; }
    QString skillId() const { return skillId_; }
    int score() const { return score_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    void setId(int id) { id_ = id; }
    void setEngineerId(const QString& engineerId) { engineerId_ = engineerId; }
    void setCategoryId(const QString& categoryId) { categoryId_ = categoryId; }
    void setSkillId(const QString& skillId) { skillId_ = skillId; }
    void setScore(int score) { score_ = score; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    bool isValid() const;
    QJsonObject toJson() const;
    static CoreSkillAssessment fromJson(const QJsonObject& json);

private:
    int id_;
    QString engineerId_;
    QString categoryId_;
    QString skillId_;
    int score_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // CORESKILLASSESSMENT_H
