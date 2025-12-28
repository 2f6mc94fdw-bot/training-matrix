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

    // Multi-Criteria Weighting Getters (0.0 - 5.0 scale)
    double safetyImpact() const { return safetyImpact_; }
    double productionImpact() const { return productionImpact_; }
    double frequency() const { return frequency_; }
    double complexity() const { return complexity_; }
    double futureValue() const { return futureValue_; }

    // Calculated weight based on multi-criteria
    double calculatedWeight() const;

    // Setters
    void setId(int id) { id_ = id; }
    void setMachineId(int machineId) { machineId_ = machineId; }
    void setName(const QString& name) { name_ = name; }
    void setMaxScore(int maxScore) { maxScore_ = maxScore; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Multi-Criteria Weighting Setters
    void setSafetyImpact(double value) { safetyImpact_ = value; }
    void setProductionImpact(double value) { productionImpact_ = value; }
    void setFrequency(double value) { frequency_ = value; }
    void setComplexity(double value) { complexity_ = value; }
    void setFutureValue(double value) { futureValue_ = value; }

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

    // Multi-Criteria Weighting (0.0 - 5.0 scale, default 3.0)
    double safetyImpact_;       // 30% weight - Risk if competency lacking
    double productionImpact_;   // 25% weight - Effect on output/quality
    double frequency_;          // 20% weight - How often used
    double complexity_;         // 15% weight - Difficulty to master
    double futureValue_;        // 10% weight - Career/strategic importance
};

#endif // COMPETENCY_H
