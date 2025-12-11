#include "Crypto.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>
#include <QUuid>

QString Crypto::hashPassword(const QString& password)
{
    // For production, use BCrypt or Argon2
    // This is a simple implementation using SHA-256 with multiple rounds
    QString hashed = password;

    // Apply multiple rounds of hashing for better security
    for (int i = 0; i < 10; i++) {
        QByteArray data = hashed.toUtf8();
        QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
        hashed = hash.toHex();
    }

    return hashed;
}

bool Crypto::verifyPassword(const QString& password, const QString& hash)
{
    QString hashedInput = hashPassword(password);
    return hashedInput == hash;
}

QString Crypto::generateSalt(int length)
{
    QByteArray salt;
    salt.resize(length);

    for (int i = 0; i < length; i++) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    return salt.toHex();
}

QString Crypto::hashPasswordWithSalt(const QString& password, const QString& salt)
{
    QString salted = salt + password + salt;
    return hashPassword(salted);
}

QString Crypto::generateId(const QString& prefix)
{
    // Generate unique ID using timestamp and random component
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    quint32 random = QRandomGenerator::global()->generate();

    QString id = QString("%1%2_%3")
        .arg(prefix.isEmpty() ? "" : prefix + "_")
        .arg(timestamp)
        .arg(random, 8, 16, QChar('0'));

    return id;
}

QString Crypto::sha256(const QString& data)
{
    QByteArray bytes = data.toUtf8();
    QByteArray hash = QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);
    return hash.toHex();
}
