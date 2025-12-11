#include "ValidationHelper.h"
#include "../core/Constants.h"

bool ValidationHelper::isValidUsername(const QString& username)
{
    if (username.trimmed().isEmpty()) {
        return false;
    }

    if (username.length() < 3 || username.length() > 100) {
        return false;
    }

    // Username should contain only alphanumeric characters, underscore, and dash
    QRegularExpression regex("^[a-zA-Z0-9_-]+$");
    return regex.match(username).hasMatch();
}

bool ValidationHelper::isValidPassword(const QString& password)
{
    if (password.isEmpty()) {
        return false;
    }

    if (password.length() < Constants::PASSWORD_MIN_LENGTH ||
        password.length() > Constants::PASSWORD_MAX_LENGTH) {
        return false;
    }

    return true;
}

bool ValidationHelper::isValidEmail(const QString& email)
{
    if (email.trimmed().isEmpty()) {
        return false;
    }

    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex.match(email).hasMatch();
}

bool ValidationHelper::isValidScore(int score, int maxScore)
{
    return score >= 0 && score <= maxScore;
}

bool ValidationHelper::isValidImportance(int importance)
{
    return importance >= 1 && importance <= 10;
}

bool ValidationHelper::isEmpty(const QString& str)
{
    return str.trimmed().isEmpty();
}

bool ValidationHelper::validateRequired(const QString& value, const QString& fieldName, QString& errorMessage)
{
    if (isEmpty(value)) {
        errorMessage = QString("%1 is required").arg(fieldName);
        return false;
    }
    return true;
}

bool ValidationHelper::validateLength(const QString& value, int minLength, int maxLength,
                                     const QString& fieldName, QString& errorMessage)
{
    int length = value.length();

    if (length < minLength) {
        errorMessage = QString("%1 must be at least %2 characters").arg(fieldName).arg(minLength);
        return false;
    }

    if (length > maxLength) {
        errorMessage = QString("%1 must be no more than %2 characters").arg(fieldName).arg(maxLength);
        return false;
    }

    return true;
}

bool ValidationHelper::validateRange(int value, int min, int max,
                                    const QString& fieldName, QString& errorMessage)
{
    if (value < min || value > max) {
        errorMessage = QString("%1 must be between %2 and %3").arg(fieldName).arg(min).arg(max);
        return false;
    }
    return true;
}

QString ValidationHelper::sanitize(const QString& str)
{
    QString sanitized = str;

    // Remove potentially dangerous characters
    sanitized.remove(QChar('\0')); // Null character
    sanitized.remove(QChar('\r')); // Carriage return
    sanitized.remove(QChar('\n')); // Newline (optional - may want to keep in some cases)

    // Trim whitespace
    sanitized = sanitized.trimmed();

    return sanitized;
}
