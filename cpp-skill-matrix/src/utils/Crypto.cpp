#include "Crypto.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>
#include <QUuid>

QString Crypto::hashPassword(const QString& password)
{
    // IMPORTANT: For production, use BCrypt or Argon2 libraries
    // This implementation uses PBKDF2-like approach with SHA-256
    // It provides better security than plain hashing but is not as strong as BCrypt/Argon2

    // Generate a unique salt per password (stored as prefix in the hash)
    QString salt = generateSalt(16); // 16 bytes = 32 hex chars

    // Perform PBKDF2-like derivation with 10000 iterations
    QByteArray derived = password.toUtf8() + QByteArray::fromHex(salt.toUtf8());

    // Apply 10000 rounds of hashing (PBKDF2-like)
    for (int i = 0; i < 10000; i++) {
        QCryptographicHash hasher(QCryptographicHash::Sha256);
        hasher.addData(derived);
        hasher.addData(QByteArray::number(i)); // Include iteration count
        derived = hasher.result();
    }

    // Return format: salt$hash (allows verification with same salt)
    return salt + "$" + derived.toHex();
}

bool Crypto::verifyPassword(const QString& password, const QString& hash)
{
    // Check if hash contains salt separator (new format: salt$hash)
    if (hash.contains("$")) {
        // New format with salt
        QStringList parts = hash.split("$");
        if (parts.size() != 2) {
            return false;
        }

        QString salt = parts[0];
        QString storedHash = parts[1];

        // Derive hash with same salt
        QByteArray derived = password.toUtf8() + QByteArray::fromHex(salt.toUtf8());

        for (int i = 0; i < 10000; i++) {
            QCryptographicHash hasher(QCryptographicHash::Sha256);
            hasher.addData(derived);
            hasher.addData(QByteArray::number(i));
            derived = hasher.result();
        }

        return derived.toHex() == storedHash;
    } else {
        // Legacy format without salt (for backward compatibility)
        // This is the old insecure method - kept only for existing passwords
        QString hashed = password;
        for (int i = 0; i < 10; i++) {
            QByteArray data = hashed.toUtf8();
            QByteArray hashBytes = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
            hashed = hashBytes.toHex();
        }
        return hashed == hash;
    }
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
