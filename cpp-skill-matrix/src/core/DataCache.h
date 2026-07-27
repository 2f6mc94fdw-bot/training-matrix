#ifndef DATACACHE_H
#define DATACACHE_H

#include <QObject>
#include <QDateTime>
#include <QFutureWatcher>
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
    bool isLoading() const { return isLoading_; }
    void load();
    void loadAsync();
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
    struct AsyncLoadResult {
        ProductionHierarchy hierarchy;
        QString error;
        bool success = false;
        quint64 requestId = 0;
    };

    DataCache();
    ~DataCache();
    DataCache(const DataCache&) = delete;
    DataCache& operator=(const DataCache&) = delete;
    void applyHierarchy(const ProductionHierarchy& hierarchy);

    ProductionRepository repository_;
    ProductionHierarchy hierarchy_;
    bool isLoaded_;
    bool isLoading_;
    QDateTime lastLoadTime_;
    QString lastError_;
    QFutureWatcher<AsyncLoadResult>* loadWatcher_;
    quint64 asyncRequestCounter_;
    quint64 expectedAsyncRequestId_;

    // Quick lookup maps
    QHash<int, ProductionArea> areasById_;
    QHash<int, Machine> machinesById_;
    QHash<int, Competency> competenciesById_;
};

#endif // DATACACHE_H
