#ifndef SNAPSHOTCONTROLLER_H
#define SNAPSHOTCONTROLLER_H

#include "../models/Snapshot.h"
#include <QList>
#include <QString>
#include <QDateTime>

/**
 * @brief Controller for Snapshot business logic
 */
class SnapshotController
{
public:
    SnapshotController();
    ~SnapshotController();

    QList<Snapshot> getAllSnapshots(int limit = 50);
    Snapshot getSnapshotById(const QString& id);
    QString createSnapshot(const QString& description, const QString& jsonData);
    bool deleteSnapshot(const QString& id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // SNAPSHOTCONTROLLER_H
