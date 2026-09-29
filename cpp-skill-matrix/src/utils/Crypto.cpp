#include "Crypto.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>
#include <QUuid>
#include <QStringList>

namespace {
constexpr int kCurrentIterations = 120000;
constexpr int kLegacyIterations = 10000;
const QString kCurrentPrefix = "sm2";
}

QString Crypto::hashPassword(const QString& password)
{
    // Versioned format for migration support:
    // sm2$<iterations>$<salt_hex>$<derived_hex>
    const QString salt = generateSalt(16);
    const QByteArray derived = derivePasswordHash(password, salt, kCurrentIterations);
    return QString("%1$%2$%3$%4")
        .arg(kCurrentPrefix)
        .arg(kCurrentIterations)
        .arg(salt)
        .arg(QString::fromUtf8(derived.toHex()));
}

bool Crypto::verifyPassword(const QString& password, const QString& hash)
{
    // Current format: sm2$iterations$salt$hash
    if (hash.startsWith(kCurrentPrefix + "$")) {
        const QStringList parts = hash.split("$");
        if (parts.size() != 4) {
            return false;
        }
        bool ok = false;
        const int iterations = parts[1].toInt(&ok);
        if (!ok || iterations < 1000) {
            return false;
        }
        const QByteArray derived = derivePasswordHash(password, parts[2], iterations);
        return constantTimeEquals(derived.toHex(), parts[3].toUtf8());
    }

    // Previous salted format: salt$hash
    if (hash.contains("$")) {
        const QStringList parts = hash.split("$");
        if (parts.size() != 2) {
            return false;
        }
        const QByteArray derived = derivePasswordHash(password, parts[0], kLegacyIterations);
        return constantTimeEquals(derived.toHex(), parts[1].toUtf8());
    } else {
        // Old unsalted format kept for backward compatibility
        QString hashed = password;
        for (int i = 0; i < 10; i++) {
            QByteArray data = hashed.toUtf8();
            QByteArray hashBytes = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
            hashed = hashBytes.toHex();
        }
        return constantTimeEquals(hashed.toUtf8(), hash.toUtf8());
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

bool Crypto::needsRehash(const QString& storedHash)
{
    if (!storedHash.startsWith(kCurrentPrefix + "$")) {
        return true;
    }

    const QStringList parts = storedHash.split("$");
    if (parts.size() != 4) {
        return true;
    }

    bool ok = false;
    const int iterations = parts[1].toInt(&ok);
    return !ok || iterations < kCurrentIterations;
}

QByteArray Crypto::derivePasswordHash(const QString& password, const QString& saltHex, int iterations)
{
    QByteArray derived = password.toUtf8() + QByteArray::fromHex(saltHex.toUtf8());
    for (int i = 0; i < iterations; ++i) {
        QCryptographicHash hasher(QCryptographicHash::Sha256);
        hasher.addData(derived);
        hasher.addData(QByteArray::number(i));
        derived = hasher.result();
    }
    return derived;
}

bool Crypto::constantTimeEquals(const QByteArray& a, const QByteArray& b)
{
    const int maxSize = qMax(a.size(), b.size());
    quint8 diff = static_cast<quint8>(a.size() ^ b.size());

    for (int i = 0; i < maxSize; ++i) {
        const quint8 av = i < a.size() ? static_cast<quint8>(a.at(i)) : 0;
        const quint8 bv = i < b.size() ? static_cast<quint8>(b.at(i)) : 0;
        diff |= static_cast<quint8>(av ^ bv);
    }

    return diff == 0;
}
