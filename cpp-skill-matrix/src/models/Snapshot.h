#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Snapshot
{
public:
    Snapshot();
    ~Snapshot();

    QString id() const { return id_; }
    QString description() const { return description_; }
    QDateTime timestamp() const { return timestamp_; }
    QString data() const { return data_; }
    QDateTime createdAt() const { return createdAt_; }

    void setId(const QString& id) { id_ = id; }
    void setDescription(const QString& description) { description_ = description; }
    void setTimestamp(const QDateTime& timestamp) { timestamp_ = timestamp; }
    void setData(const QString& data) { data_ = data; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    bool isValid() const;
    QJsonObject toJson() const;
    static Snapshot fromJson(const QJsonObject& json);

private:
    QString id_;
    QString description_;
    QDateTime timestamp_;
    QString data_; // JSON string
    QDateTime createdAt_;
};

#endif // SNAPSHOT_H
