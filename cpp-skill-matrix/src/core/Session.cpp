#include "Session.h"
#include "Constants.h"

Session::Session(QObject* parent)
    : QObject(parent)
    , isLoggedIn_(false)
    , userId_("")
    , username_("")
    , role_("")
    , engineerId_("")
    , loginTime_()
    , lastActivity_()
{
}

Session::~Session()
{
}

void Session::setLoggedIn(bool loggedIn)
{
    if (isLoggedIn_ != loggedIn) {
        isLoggedIn_ = loggedIn;

        if (loggedIn) {
            loginTime_ = QDateTime::currentDateTime();
            lastActivity_ = QDateTime::currentDateTime();
            emit sessionStarted();
        } else {
            emit sessionEnded();
        }
    }
}

void Session::setUserId(const QString& userId)
{
    userId_ = userId;
}

void Session::setUsername(const QString& username)
{
    username_ = username;
}

void Session::setRole(const QString& role)
{
    role_ = role;
}

void Session::setEngineerId(const QString& engineerId)
{
    engineerId_ = engineerId;
}

bool Session::isAdmin() const
{
    return role_ == Constants::ROLE_ADMIN;
}

bool Session::isEngineer() const
{
    return role_ == Constants::ROLE_ENGINEER;
}

void Session::updateLastActivity()
{
    lastActivity_ = QDateTime::currentDateTime();
}

bool Session::hasTimedOut(int timeoutMs) const
{
    if (!isLoggedIn_) {
        return false;
    }

    qint64 elapsedMs = lastActivity_.msecsTo(QDateTime::currentDateTime());
    return elapsedMs > timeoutMs;
}

void Session::clear()
{
    bool wasLoggedIn = isLoggedIn_;

    isLoggedIn_ = false;
    userId_ = "";
    username_ = "";
    role_ = "";
    engineerId_ = "";
    loginTime_ = QDateTime();
    lastActivity_ = QDateTime();

    if (wasLoggedIn) {
        emit sessionEnded();
    }
}
