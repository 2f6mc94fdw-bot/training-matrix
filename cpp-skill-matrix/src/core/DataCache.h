#ifndef DATACACHE_H
#define DATACACHE_H

#include <QObject>
#include <QDateTime>
#include "../database/ProductionRepository.h"
#include "../models/ProductionArea.h"
#include "../models/Machine.h"
#include "../models/Competency.h"

/**
 * @brief Global data cache for production hierarchy
 *
 * This singleton provides fast, in-memory access to production data
 * that rarely changes. Eliminates N+1 query problems across all widgets.
 *
 * Usage:
 *   DataCache& cache = DataCache::instance();
 *   if (!cache.isLoaded()) {
 *       cache.load();
 *   }
 *   QList<ProductionArea> areas = cache.getAreas();
 */
class DataCache : public QObject
{
    Q_OBJECT

public:
    static DataCache& instance();

    // Cache lifecycle
    bool isLoaded() const { return isLoaded_; }
    void load();
    void refresh();
    void invalidate();

    // Data access (fast, in-memory)
    QList<ProductionArea> getAreas() const;
    QList<Machine> getMachinesByArea(int areaId) const;
    QList<Competency> getCompetenciesByMachine(int machineId) const;
    ProductionArea getAreaById(int areaId) const;
    Machine getMachineById(int machineId) const;
    Competency getCompetencyById(int competencyId) const;

    // Statistics
    int getTotalAreas() const { return hierarchy_.areas.size(); }
    int getTotalMachines() const;
    int getTotalCompetencies() const;
    QDateTime getLastLoadTime() const { return lastLoadTime_; }

    QString lastError() const { return lastError_; }

signals:
    void loadStarted();
    void loadCompleted(bool success);
    void cacheInvalidated();

private:
    DataCache();
    ~DataCache();
    DataCache(const DataCache&) = delete;
    DataCache& operator=(const DataCache&) = delete;

    ProductionRepository repository_;
    ProductionHierarchy hierarchy_;
    bool isLoaded_;
    QDateTime lastLoadTime_;
    QString lastError_;

    // Quick lookup maps
    QHash<int, ProductionArea> areasById_;
    QHash<int, Machine> machinesById_;
    QHash<int, Competency> competenciesById_;
};

#endif // DATACACHE_H
