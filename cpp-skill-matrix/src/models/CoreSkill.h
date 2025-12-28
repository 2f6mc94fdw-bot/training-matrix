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

    // Multi-Criteria Weighting Getters (0.0 - 5.0 scale)
    double safetyImpact() const { return safetyImpact_; }
    double productionImpact() const { return productionImpact_; }
    double frequency() const { return frequency_; }
    double complexity() const { return complexity_; }
    double futureValue() const { return futureValue_; }

    // Calculated weight based on multi-criteria
    double calculatedWeight() const;

    void setId(const QString& id) { id_ = id; }
    void setCategoryId(const QString& categoryId) { categoryId_ = categoryId; }
    void setName(const QString& name) { name_ = name; }
    void setMaxScore(int maxScore) { maxScore_ = maxScore; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    // Multi-Criteria Weighting Setters
    void setSafetyImpact(double value) { safetyImpact_ = value; }
    void setProductionImpact(double value) { productionImpact_ = value; }
    void setFrequency(double value) { frequency_ = value; }
    void setComplexity(double value) { complexity_ = value; }
    void setFutureValue(double value) { futureValue_ = value; }

    bool isValid() const;
    QJsonObject toJson() const;
    static CoreSkill fromJson(const QJsonObject& json);

private:
    QString id_;
    QString categoryId_;
    QString name_;
    int maxScore_;
    QDateTime createdAt_;

    // Multi-Criteria Weighting (0.0 - 5.0 scale, default 3.0)
    double safetyImpact_;       // 30% weight - Risk if competency lacking
    double productionImpact_;   // 25% weight - Effect on output/quality
    double frequency_;          // 20% weight - How often used
    double complexity_;         // 15% weight - Difficulty to master
    double futureValue_;        // 10% weight - Career/strategic importance
};

#endif // CORESKILL_H
