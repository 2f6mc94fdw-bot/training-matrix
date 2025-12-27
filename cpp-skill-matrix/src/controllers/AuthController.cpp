#include "AuthController.h"
#include "../core/Application.h"
#include "../database/UserRepository.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"

AuthController::AuthController() {}
AuthController::~AuthController() {}

bool AuthController::login(const QString& username, const QString& password) { return false; }
void AuthController::logout() {}

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
