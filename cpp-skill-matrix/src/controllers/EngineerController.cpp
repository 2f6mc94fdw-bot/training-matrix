#include "EngineerController.h"
#include "../database/EngineerRepository.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"
#include "../core/Constants.h"

EngineerController::EngineerController() : lastError_("") {}
EngineerController::~EngineerController() {}

QList<Engineer> EngineerController::getAllEngineers()
{
    lastError_.clear();
    EngineerRepository repo;
    QList<Engineer> engineers = repo.findAll();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to get engineers: " + lastError_);
    }

    return engineers;
}

QList<Engineer> EngineerController::getEngineersByShift(const QString& shift)
{
    lastError_.clear();
    EngineerRepository repo;
    QList<Engineer> engineers = repo.findByShift(shift);

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to get engineers by shift: " + lastError_);
    }

    return engineers;
}

Engineer EngineerController::getEngineerById(const QString& id)
{
    lastError_.clear();
    EngineerRepository repo;
    Engineer engineer = repo.findById(id);

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to get engineer: " + lastError_);
    }

    return engineer;
}

QString EngineerController::createEngineer(const QString& name, const QString& shift)
{
    lastError_.clear();

    // Validate input
    if (!validateEngineer(name, shift)) {
        return QString(); // Error already set by validateEngineer
    }

    // Create engineer object
    Engineer engineer;
    engineer.setId(Crypto::generateId("eng"));
    engineer.setName(name);
    engineer.setShift(shift);

    // Save to repository
    EngineerRepository repo;
    bool success = repo.save(engineer);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to create engineer: " + lastError_);
        return QString();
    }

    Logger::instance().info("EngineerController", QString("Created engineer: %1 (%2)").arg(name).arg(engineer.id()));
    return engineer.id();
}

bool EngineerController::updateEngineer(const QString& id, const QString& name, const QString& shift)
{
    lastError_.clear();

    // Validate input
    if (id.isEmpty()) {
        lastError_ = "Engineer ID cannot be empty";
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    if (!validateEngineer(name, shift)) {
        return false; // Error already set by validateEngineer
    }

    // Get existing engineer
    EngineerRepository repo;
    Engineer engineer = repo.findById(id);

    if (!engineer.isValid()) {
        lastError_ = "Engineer not found: " + id;
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    // Update engineer
    engineer.setName(name);
    engineer.setShift(shift);

    bool success = repo.update(engineer);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to update engineer: " + lastError_);
    } else {
        Logger::instance().info("EngineerController", QString("Updated engineer: %1").arg(id));
    }

    return success;
}

bool EngineerController::deleteEngineer(const QString& id)
{
    lastError_.clear();

    if (id.isEmpty()) {
        lastError_ = "Engineer ID cannot be empty";
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    EngineerRepository repo;
    bool success = repo.remove(id);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("EngineerController", "Failed to delete engineer: " + lastError_);
    } else {
        Logger::instance().info("EngineerController", QString("Deleted engineer: %1").arg(id));
    }

    return success;
}

bool EngineerController::validateEngineer(const QString& name, const QString& shift)
{
    // Validate name
    if (name.trimmed().isEmpty()) {
        lastError_ = "Engineer name cannot be empty";
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    if (name.length() > 200) {
        lastError_ = "Engineer name is too long (max 200 characters)";
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    // Validate shift
    if (shift.trimmed().isEmpty()) {
        lastError_ = "Shift cannot be empty";
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    // Validate shift is one of the allowed values
    QStringList validShifts = {
        Constants::SHIFT_A,
        Constants::SHIFT_B,
        Constants::SHIFT_C,
        Constants::SHIFT_D,
        Constants::SHIFT_DAY
    };

    if (!validShifts.contains(shift)) {
        lastError_ = QString("Invalid shift: %1. Must be one of: %2")
            .arg(shift)
            .arg(validShifts.join(", "));
        Logger::instance().warning("EngineerController", lastError_);
        return false;
    }

    return true;
}
