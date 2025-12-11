#ifndef ASSESSMENT_H
#define ASSESSMENT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Assessment model class
 */
class Assessment
{
public:
    Assessment();
    Assessment(int id, const QString& engineerId, int productionAreaId,
               int machineId, int competencyId, int score);
    ~Assessment();

    // Getters
    int id() const { return id_; }
    QString engineerId() const { return engineerId_; }
    int productionAreaId() const { return productionAreaId_; }
    int machineId() const { return machineId_; }
    int competencyId() const { return competencyId_; }
    int score() const { return score_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setEngineerId(const QString& engineerId) { engineerId_ = engineerId; }
    void setProductionAreaId(int productionAreaId) { productionAreaId_ = productionAreaId; }
    void setMachineId(int machineId) { machineId_ = machineId; }
    void setCompetencyId(int competencyId) { competencyId_ = competencyId; }
    void setScore(int score) { score_ = score; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Assessment fromJson(const QJsonObject& json);

    // Key generation (for uniqueness)
    QString getKey() const;
    static QString makeKey(const QString& engineerId, int productionAreaId,
                          int machineId, int competencyId);

    // Equality
    bool operator==(const Assessment& other) const { return id_ == other.id_; }

private:
    int id_;
    QString engineerId_;
    int productionAreaId_;
    int machineId_;
    int competencyId_;
    int score_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // ASSESSMENT_H
