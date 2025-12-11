#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>

/**
 * @brief Cryptography utility class for password hashing and verification
 *
 * Note: This implementation uses Qt's SHA-256 for simplicity.
 * For production, consider using BCrypt or Argon2 for password hashing.
 */
class Crypto
{
public:
    /**
     * @brief Hash a password
     * @param password Plain text password
     * @return Hashed password (hex string)
     */
    static QString hashPassword(const QString& password);

    /**
     * @brief Verify a password against a hash
     * @param password Plain text password
     * @param hash Hashed password to compare against
     * @return true if password matches hash
     */
    static bool verifyPassword(const QString& password, const QString& hash);

    /**
     * @brief Generate a random salt
     * @param length Length of salt in bytes
     * @return Random salt (hex string)
     */
    static QString generateSalt(int length = 16);

    /**
     * @brief Hash a password with salt
     * @param password Plain text password
     * @param salt Salt value
     * @return Hashed password (hex string)
     */
    static QString hashPasswordWithSalt(const QString& password, const QString& salt);

    /**
     * @brief Generate a random ID
     * @param prefix Optional prefix for the ID
     * @return Random ID string
     */
    static QString generateId(const QString& prefix = QString());

    /**
     * @brief Hash a string using SHA-256
     * @param data Data to hash
     * @return Hash (hex string)
     */
    static QString sha256(const QString& data);

private:
    Crypto() = delete; // Static class
};

#endif // CRYPTO_H
