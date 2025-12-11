#ifndef PRODUCTIONAREA_H
#define PRODUCTIONAREA_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QList>

class Machine;

/**
 * @brief ProductionArea model class
 */
class ProductionArea
{
public:
    ProductionArea();
    ProductionArea(int id, const QString& name);
    ~ProductionArea();

    // Getters
    int id() const { return id_; }
    QString name() const { return name_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setName(const QString& name) { name_ = name; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static ProductionArea fromJson(const QJsonObject& json);

    // Equality
    bool operator==(const ProductionArea& other) const { return id_ == other.id_; }

private:
    int id_;
    QString name_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // PRODUCTIONAREA_H
