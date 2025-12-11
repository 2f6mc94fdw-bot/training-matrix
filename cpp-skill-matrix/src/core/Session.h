#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QString>
#include <QDateTime>

/**
 * @brief Session class for managing user session data
 *
 * Stores current user information and session state
 */
class Session : public QObject
{
    Q_OBJECT

public:
    explicit Session(QObject* parent = nullptr);
    ~Session();

    // Getters
    bool isLoggedIn() const { return isLoggedIn_; }
    QString userId() const { return userId_; }
    QString username() const { return username_; }
    QString role() const { return role_; }
    QString engineerId() const { return engineerId_; }
    QDateTime loginTime() const { return loginTime_; }
    QDateTime lastActivity() const { return lastActivity_; }

    // Setters
    void setLoggedIn(bool loggedIn);
    void setUserId(const QString& userId);
    void setUsername(const QString& username);
    void setRole(const QString& role);
    void setEngineerId(const QString& engineerId);

    /**
     * @brief Check if user is admin
     * @return true if admin
     */
    bool isAdmin() const;

    /**
     * @brief Check if user is engineer
     * @return true if engineer
     */
    bool isEngineer() const;

    /**
     * @brief Update last activity timestamp
     */
    void updateLastActivity();

    /**
     * @brief Check if session has timed out
     * @param timeoutMs Timeout in milliseconds
     * @return true if session has timed out
     */
    bool hasTimedOut(int timeoutMs) const;

    /**
     * @brief Clear session data (logout)
     */
    void clear();

signals:
    /**
     * @brief Emitted when session becomes active
     */
    void sessionStarted();

    /**
     * @brief Emitted when session ends
     */
    void sessionEnded();

    /**
     * @brief Emitted when session times out
     */
    void sessionTimedOut();

private:
    bool isLoggedIn_;
    QString userId_;
    QString username_;
    QString role_;
    QString engineerId_;
    QDateTime loginTime_;
    QDateTime lastActivity_;
};

#endif // SESSION_H
