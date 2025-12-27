#include "SnapshotController.h"
#include "../database/SnapshotRepository.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"

SnapshotController::SnapshotController() : lastError_("") {}
SnapshotController::~SnapshotController() {}

QList<Snapshot> SnapshotController::getAllSnapshots(int limit)
{
    lastError_.clear();
    SnapshotRepository repo;
    return repo.findAll(limit);
}

Snapshot SnapshotController::getSnapshotById(const QString& id)
{
    lastError_.clear();
    SnapshotRepository repo;
    return repo.findById(id);
}

QString SnapshotController::createSnapshot(const QString& description, const QString& jsonData)
{
    lastError_.clear();

    if (jsonData.isEmpty()) {
        lastError_ = "Snapshot data cannot be empty";
        return QString();
    }

    Snapshot snapshot;
    snapshot.setId(Crypto::generateId("snapshot"));
    snapshot.setDescription(description);
    snapshot.setTimestamp(QDateTime::currentDateTime());
    snapshot.setData(jsonData);

    SnapshotRepository repo;
    bool success = repo.save(snapshot);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("SnapshotController", "Failed to create snapshot: " + lastError_);
        return QString();
    }

    Logger::instance().info("SnapshotController", "Created snapshot: " + snapshot.id());
    return snapshot.id();
}

bool SnapshotController::deleteSnapshot(const QString& id)
{
    lastError_.clear();

    if (id.isEmpty()) {
        lastError_ = "Snapshot ID cannot be empty";
        return false;
    }

    SnapshotRepository repo;
    bool success = repo.remove(id);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}
