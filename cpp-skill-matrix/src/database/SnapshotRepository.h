#ifndef SNAPSHOTREPOSITORY_H
#define SNAPSHOTREPOSITORY_H

#include "../models/Snapshot.h"
#include <QList>

class SnapshotRepository
{
public:
    SnapshotRepository();
    ~SnapshotRepository();

    QList<Snapshot> findAll(int limit = 50);
    Snapshot findById(const QString& id);
    bool save(Snapshot& snapshot);
    bool remove(const QString& id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // SNAPSHOTREPOSITORY_H
