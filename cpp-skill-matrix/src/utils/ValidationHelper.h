#ifndef VALIDATIONHELPER_H
#define VALIDATIONHELPER_H

#include <QString>
#include <QRegularExpression>

/**
 * @brief Helper class for input validation
 */
class ValidationHelper
{
public:
    /**
     * @brief Validate username
     * @param username Username to validate
     * @return true if valid
     */
    static bool isValidUsername(const QString& username);

    /**
     * @brief Validate password
     * @param password Password to validate
     * @return true if valid
     */
    static bool isValidPassword(const QString& password);

    /**
     * @brief Validate email address
     * @param email Email to validate
     * @return true if valid
     */
    static bool isValidEmail(const QString& email);

    /**
     * @brief Validate score value
     * @param score Score to validate
     * @param maxScore Maximum allowed score
     * @return true if valid
     */
    static bool isValidScore(int score, int maxScore = 3);

    /**
     * @brief Validate importance value
     * @param importance Importance to validate
     * @return true if valid (1-10)
     */
    static bool isValidImportance(int importance);

    /**
     * @brief Check if string is empty or whitespace
     * @param str String to check
     * @return true if empty/whitespace
     */
    static bool isEmpty(const QString& str);

    /**
     * @brief Validate required field
     * @param value Value to check
     * @param fieldName Field name for error message
     * @param errorMessage Output error message
     * @return true if valid
     */
    static bool validateRequired(const QString& value, const QString& fieldName, QString& errorMessage);

    /**
     * @brief Validate string length
     * @param value String to validate
     * @param minLength Minimum length
     * @param maxLength Maximum length
     * @param fieldName Field name for error message
     * @param errorMessage Output error message
     * @return true if valid
     */
    static bool validateLength(const QString& value, int minLength, int maxLength,
                              const QString& fieldName, QString& errorMessage);

    /**
     * @brief Validate integer range
     * @param value Value to validate
     * @param min Minimum value
     * @param max Maximum value
     * @param fieldName Field name for error message
     * @param errorMessage Output error message
     * @return true if valid
     */
    static bool validateRange(int value, int min, int max,
                             const QString& fieldName, QString& errorMessage);

    /**
     * @brief Sanitize string (remove dangerous characters)
     * @param str String to sanitize
     * @return Sanitized string
     */
    static QString sanitize(const QString& str);

private:
    ValidationHelper() = delete; // Static class
};

#endif // VALIDATIONHELPER_H
