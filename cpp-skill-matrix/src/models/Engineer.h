#ifndef ENGINEER_H
#define ENGINEER_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Engineer model class
 */
class Engineer
{
public:
    Engineer();
    Engineer(const QString& id, const QString& name, const QString& shift);
    ~Engineer();

    // Getters
    QString id() const { return id_; }
    QString name() const { return name_; }
    QString shift() const { return shift_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(const QString& id) { id_ = id; }
    void setName(const QString& name) { name_ = name; }
    void setShift(const QString& shift) { shift_ = shift; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Engineer fromJson(const QJsonObject& json);

    // Equality
    bool operator==(const Engineer& other) const { return id_ == other.id_; }

private:
    QString id_;
    QString name_;
    QString shift_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // ENGINEER_H
