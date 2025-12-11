#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @brief User model class
 */
class User
{
public:
    User();
    User(const QString& id, const QString& username, const QString& password, const QString& role);
    ~User();

    // Getters
    QString id() const { return id_; }
    QString username() const { return username_; }
    QString password() const { return password_; }
    QString role() const { return role_; }
    QString engineerId() const { return engineerId_; }
    QDateTime createdAt() const { return createdAt_; }
    QDateTime updatedAt() const { return updatedAt_; }

    // Setters
    void setId(const QString& id) { id_ = id; }
    void setUsername(const QString& username) { username_ = username; }
    void setPassword(const QString& password) { password_ = password; }
    void setRole(const QString& role) { role_ = role; }
    void setEngineerId(const QString& engineerId) { engineerId_ = engineerId; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }
    void setUpdatedAt(const QDateTime& updatedAt) { updatedAt_ = updatedAt; }

    // Validation
    bool isValid() const;
    bool isAdmin() const;
    bool isEngineer() const;

    // Serialization
    QJsonObject toJson() const;
    static User fromJson(const QJsonObject& json);

    // Equality
    bool operator==(const User& other) const { return id_ == other.id_; }

private:
    QString id_;
    QString username_;
    QString password_; // Hashed
    QString role_;
    QString engineerId_; // Optional: link to engineer record
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

#endif // USER_H
