#include "SnapshotRepository.h"

SnapshotRepository::SnapshotRepository() : lastError_("") {}
SnapshotRepository::~SnapshotRepository() {}

QList<Snapshot> SnapshotRepository::findAll(int limit) { return QList<Snapshot>(); }
Snapshot SnapshotRepository::findById(const QString& id) { return Snapshot(); }
bool SnapshotRepository::save(Snapshot& snapshot) { return false; }
bool SnapshotRepository::remove(const QString& id) { return false; }
