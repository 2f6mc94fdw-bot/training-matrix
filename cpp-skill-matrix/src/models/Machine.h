#ifndef MACHINE_H
#define MACHINE_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief Machine model class
 */
class Machine
{
public:
    Machine();
    Machine(int id, int productionAreaId, const QString& name, int importance = 1);
    ~Machine();

    // Getters
    int id() const { return id_; }
    int productionAreaId() const { return productionAreaId_; }
    QString name() const { return name_; }
    int importance() const { return importance_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setProductionAreaId(int productionAreaId) { productionAreaId_ = productionAreaId; }
    void setName(const QString& name) { name_ = name; }
    void setImportance(int importance) { importance_ = importance; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Machine fromJson(const QJsonObject& json);

    // Equality
    bool operator==(const Machine& other) const { return id_ == other.id_; }

private:
    int id_;
    int productionAreaId_;
    QString name_;
    int importance_; // 1-10
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // MACHINE_H
