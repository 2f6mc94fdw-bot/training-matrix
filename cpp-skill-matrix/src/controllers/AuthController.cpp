#include "AuthController.h"
#include "../core/Application.h"
#include "../database/UserRepository.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"

AuthController::AuthController() {}
AuthController::~AuthController() {}

bool AuthController::login(const QString& username, const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) {
        Logger::instance().warning("AuthController", "Login failed - empty credentials");
        return false;
    }

    // Get user from database
    UserRepository userRepo;
    User user = userRepo.findByUsername(username);

    if (!user.isValid()) {
        Logger::instance().warning("AuthController", "Login failed - user not found: " + username);
        return false;
    }

    // Verify password
    if (!Crypto::verifyPassword(password, user.password())) {
        Logger::instance().warning("AuthController", "Login failed - incorrect password for: " + username);
        return false;
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
    QString oldPasswordHash = Crypto::hashPassword(oldPassword);
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
