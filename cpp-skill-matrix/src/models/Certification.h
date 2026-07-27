#ifndef CERTIFICATION_H
#define CERTIFICATION_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>

class Certification
{
public:
    Certification();
    ~Certification();

    int id() const { return id_; }
    QString engineerId() const { return engineerId_; }
    QString name() const { return name_; }
    QDate dateEarned() const { return dateEarned_; }
    QDate expiryDate() const { return expiryDate_; }
    QString certificateFilePath() const { return certificateFilePath_; }
    QDateTime createdAt() const { return createdAt_; }

    void setId(int id) { id_ = id; }
    void setEngineerId(const QString& engineerId) { engineerId_ = engineerId; }
    void setName(const QString& name) { name_ = name; }
    void setDateEarned(const QDate& dateEarned) { dateEarned_ = dateEarned; }
    void setExpiryDate(const QDate& expiryDate) { expiryDate_ = expiryDate; }
    void setCertificateFilePath(const QString& certificateFilePath) { certificateFilePath_ = certificateFilePath; }
    void setCreatedAt(const QDateTime& createdAt) { createdAt_ = createdAt; }

    bool isValid() const;
    bool isExpired() const;
    int daysUntilExpiry() const;
    QJsonObject toJson() const;
    static Certification fromJson(const QJsonObject& json);

private:
    int id_;
    QString engineerId_;
    QString name_;
    QDate dateEarned_;
    QDate expiryDate_;
    QString certificateFilePath_;
    QDateTime createdAt_;
};

#endif // CERTIFICATION_H
