#include "AuthController.h"
#include "../core/Application.h"
#include "../database/UserRepository.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"
#include <QDateTime>
#include <QHash>

namespace {
struct LoginAttemptState {
    int failures = 0;
    QDateTime lockUntil;
};

QHash<QString, LoginAttemptState> g_attemptsByUser;
constexpr int kMaxFailuresBeforeLock = 5;
constexpr int kLockoutSeconds = 30;
}

AuthController::AuthController() {}
AuthController::~AuthController() {}

bool AuthController::login(const QString& username, const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) {
        Logger::instance().warning("AuthController", "Login failed - empty credentials");
        return false;
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();
    LoginAttemptState& state = g_attemptsByUser[username];
    if (state.lockUntil.isValid() && now < state.lockUntil) {
        const qint64 remaining = now.secsTo(state.lockUntil);
        Logger::instance().warning("AuthController",
            QString("Login temporarily locked for user: %1 (%2s remaining)")
                .arg(username)
                .arg(remaining));
        return false;
    }

    // Get user from database
    UserRepository userRepo;
    User user = userRepo.findByUsername(username);

    if (!user.isValid()) {
        Logger::instance().warning("AuthController", "Login failed - user not found: " + username);
        state.failures++;
        return false;
    }

    // Verify password
    if (!Crypto::verifyPassword(password, user.password())) {
        Logger::instance().warning("AuthController", "Login failed - incorrect password for: " + username);
        state.failures++;
        if (state.failures >= kMaxFailuresBeforeLock) {
            state.failures = 0;
            state.lockUntil = now.addSecs(kLockoutSeconds);
            Logger::instance().warning("AuthController",
                QString("User login temporarily locked after repeated failures: %1").arg(username));
        }
        return false;
    }

    g_attemptsByUser.remove(username);

    if (Crypto::needsRehash(user.password())) {
        const QString upgradedHash = Crypto::hashPassword(password);
        if (!userRepo.updatePassword(user.id(), upgradedHash)) {
            Logger::instance().warning("AuthController",
                "Login succeeded but password rehash upgrade failed: " + userRepo.lastError());
        } else {
            Logger::instance().info("AuthController", "Password hash upgraded for user: " + username);
        }
    }

    // Create session
    Session* session = Application::instance().session();
    if (session) {
        session->setUserId(user.id());
        session->setUsername(user.username());
        session->setRole(user.role());
        session->setEngineerId(user.engineerId());
        session->setLoggedIn(true);
        Logger::instance().info("AuthController", "User logged in: " + username + " (role: " + user.role() + ")");
        return true;
    }

    Logger::instance().error("AuthController", "Login failed - no session available");
    return false;
}

void AuthController::logout()
{
    Session* session = Application::instance().session();
    if (session && session->isLoggedIn()) {
        QString username = session->username();
        session->clear();
        Logger::instance().info("AuthController", "User logged out: " + username);
    }
}

bool AuthController::changePassword(const QString& oldPassword, const QString& newPassword)
{
    // Get current user from session
    Session* session = Application::instance().session();
    if (!session || !session->isLoggedIn()) {
        Logger::instance().warning("AuthController", "Change password failed - no active session");
        return false;
    }

    QString userId = session->userId();
    if (userId.isEmpty()) {
        Logger::instance().warning("AuthController", "Change password failed - invalid user ID");
        return false;
    }

    // Get user from database
    UserRepository userRepo;
    User user = userRepo.findById(userId);

    if (!user.isValid()) {
        Logger::instance().warning("AuthController", "Change password failed - user not found: " + userId);
        return false;
    }

    // Verify old password
    if (!Crypto::verifyPassword(oldPassword, user.password())) {
        Logger::instance().warning("AuthController", "Change password failed - incorrect old password for: " + user.username());
        return false;
    }

    // Hash new password
    QString newPasswordHash = Crypto::hashPassword(newPassword);

    // Update password in database
    bool success = userRepo.updatePassword(userId, newPasswordHash);

    if (success) {
        Logger::instance().info("AuthController", "Password changed successfully for user: " + user.username());
    } else {
        Logger::instance().error("AuthController", "Failed to update password: " + userRepo.lastError());
    }

    return success;
}

int AuthController::lockoutSecondsRemaining(const QString& username)
{
    if (username.isEmpty()) {
        return 0;
    }

    const auto it = g_attemptsByUser.constFind(username);
    if (it == g_attemptsByUser.constEnd()) {
        return 0;
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();
    if (!it->lockUntil.isValid() || now >= it->lockUntil) {
        return 0;
    }

    return static_cast<int>(now.secsTo(it->lockUntil));
}
