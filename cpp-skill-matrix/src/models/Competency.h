#ifndef COMPETENCY_H
#define COMPETENCY_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Competency model class
 */
class Competency
{
public:
    Competency();
    Competency(int id, int machineId, const QString& name, int maxScore = 3);
    ~Competency();

    // Getters
    int id() const { return id_; }
    int machineId() const { return machineId_; }
    QString name() const { return name_; }
    int maxScore() const { return maxScore_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setMachineId(int machineId) { machineId_ = machineId; }
    void setName(const QString& name) { name_ = name; }
    void setMaxScore(int maxScore) { maxScore_ = maxScore; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Competency fromJson(const QJsonObject& json);

    // Equality
    bool operator==(const Competency& other) const { return id_ == other.id_; }

private:
    int id_;
    int machineId_;
    QString name_;
    int maxScore_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // COMPETENCY_H
