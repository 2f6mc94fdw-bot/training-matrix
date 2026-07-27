#include "DataCache.h"

#include "../database/DatabaseManager.h"
#include "../utils/Logger.h"

#include <QtConcurrent/QtConcurrent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DataCache::DataCache()
    : isLoaded_(false)
    , isLoading_(false)
    , lastError_("")
    , loadWatcher_(new QFutureWatcher<AsyncLoadResult>(this))
    , asyncRequestCounter_(0)
    , expectedAsyncRequestId_(0)
{
    connect(loadWatcher_, &QFutureWatcher<AsyncLoadResult>::finished, this, [this]() {
        const AsyncLoadResult result = loadWatcher_->result();
        isLoading_ = false;

        if (result.requestId != expectedAsyncRequestId_) {
            Logger::instance().debug("DataCache",
                QString("Ignoring stale async cache result (request %1, expected %2)")
                    .arg(result.requestId)
                    .arg(expectedAsyncRequestId_));
            return;
        }

        if (!result.success) {
            isLoaded_ = false;
            lastError_ = result.error;
            Logger::instance().error("DataCache", "Async load failed: " + lastError_);
            emit loadCompleted(false);
            return;
        }

        applyHierarchy(result.hierarchy);
        emit loadCompleted(true);
    });
}

DataCache::~DataCache()
{
    if (loadWatcher_ && loadWatcher_->isRunning()) {
        loadWatcher_->waitForFinished();
    }
}

DataCache& DataCache::instance()
{
    static DataCache instance;
    return instance;
}

void DataCache::applyHierarchy(const ProductionHierarchy& hierarchy)
{
    hierarchy_ = hierarchy;

    areasById_.clear();
    for (const ProductionArea& area : hierarchy_.areas) {
        areasById_[area.id()] = area;
    }

    machinesById_.clear();
    for (auto it = hierarchy_.machinesByArea.cbegin(); it != hierarchy_.machinesByArea.cend(); ++it) {
        for (const Machine& machine : it.value()) {
            machinesById_[machine.id()] = machine;
        }
    }

    competenciesById_.clear();
    for (auto it = hierarchy_.competenciesByMachine.cbegin(); it != hierarchy_.competenciesByMachine.cend(); ++it) {
        for (const Competency& competency : it.value()) {
            competenciesById_[competency.id()] = competency;
        }
    }

    isLoaded_ = true;
    lastLoadTime_ = QDateTime::currentDateTime();
    lastError_.clear();

    Logger::instance().info(
        "DataCache",
        QString("Cache loaded successfully: %1 areas, %2 machines, %3 competencies")
            .arg(hierarchy_.areas.size())
            .arg(machinesById_.size())
            .arg(competenciesById_.size()));
}

void DataCache::load()
{
    if (isLoaded_) {
        return;
    }

    // If async warmup is running and a caller explicitly needs data now, wait for it.
    if (isLoading_ && loadWatcher_ && loadWatcher_->isRunning()) {
        loadWatcher_->waitForFinished();
        if (isLoaded_) {
            return;
        }
    }

    Logger::instance().info("DataCache", "Loading production hierarchy...");
    emit loadStarted();

    isLoaded_ = false;
    isLoading_ = true;
    lastError_.clear();

    ProductionHierarchy hierarchy = repository_.loadCompleteHierarchy();
    isLoading_ = false;

    if (!repository_.lastError().isEmpty()) {
        lastError_ = repository_.lastError();
        Logger::instance().error("DataCache", "Failed to load hierarchy: " + lastError_);
        emit loadCompleted(false);
        return;
    }

    applyHierarchy(hierarchy);
    emit loadCompleted(true);
}

void DataCache::loadAsync()
{
    if (isLoaded_ || isLoading_) {
        return;
    }

    QSqlDatabase mainDb = DatabaseManager::instance().database();
    if (!mainDb.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().warning("DataCache", "Skipped async warmup: database not connected");
        return;
    }

    isLoading_ = true;
    isLoaded_ = false;
    lastError_.clear();
    emit loadStarted();

    const QString driver = mainDb.driverName();
    const QString databaseName = mainDb.databaseName();
    const QString connectOptions = mainDb.connectOptions();
    const quint64 requestId = ++asyncRequestCounter_;
    expectedAsyncRequestId_ = requestId;

    auto future = QtConcurrent::run([driver, databaseName, connectOptions, requestId]() -> AsyncLoadResult {
        AsyncLoadResult result;
        result.requestId = requestId;
        const QString connName = QString("DataCacheWarm_%1")
                                     .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

        {
            QSqlDatabase workerDb = QSqlDatabase::addDatabase(driver, connName);
            workerDb.setDatabaseName(databaseName);
            workerDb.setConnectOptions(connectOptions);

            if (!workerDb.open()) {
                result.error = workerDb.lastError().text();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            QSqlQuery query(workerDb);
            query.setForwardOnly(true);
            query.prepare(R"(
                SELECT
                    pa.id as area_id,
                    pa.name as area_name,
                    pa.created_at as area_created_at,
                    pa.updated_at as area_updated_at,
                    m.id as machine_id,
                    m.name as machine_name,
                    m.importance as machine_importance,
                    m.created_at as machine_created_at,
                    m.updated_at as machine_updated_at,
                    c.id as comp_id,
                    c.name as comp_name,
                    c.max_score as comp_max_score,
                    c.safety_impact as comp_safety_impact,
                    c.production_impact as comp_production_impact,
                    c.frequency as comp_frequency,
                    c.complexity as comp_complexity,
                    c.future_value as comp_future_value,
                    c.created_at as comp_created_at,
                    c.updated_at as comp_updated_at
                FROM production_areas pa
                LEFT JOIN machines m ON m.production_area_id = pa.id
                LEFT JOIN competencies c ON c.machine_id = m.id
                ORDER BY pa.name, m.name, c.name
            )");

            if (!query.exec()) {
                result.error = query.lastError().text();
                workerDb.close();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            QHash<int, ProductionArea> areasMap;
            QHash<int, Machine> machinesMap;

            while (query.next()) {
                const int areaId = query.value("area_id").toInt();

                if (!areasMap.contains(areaId)) {
                    ProductionArea area;
                    area.setId(areaId);
                    area.setName(query.value("area_name").toString());
                    area.setCreatedAt(query.value("area_created_at").toDateTime());
                    area.setUpdatedAt(query.value("area_updated_at").toDateTime());
                    areasMap[areaId] = area;
                }

                if (!query.value("machine_id").isNull()) {
                    const int machineId = query.value("machine_id").toInt();

                    if (!machinesMap.contains(machineId)) {
                        Machine machine;
                        machine.setId(machineId);
                        machine.setProductionAreaId(areaId);
                        machine.setName(query.value("machine_name").toString());
                        machine.setImportance(query.value("machine_importance").toInt());
                        machine.setCreatedAt(query.value("machine_created_at").toDateTime());
                        machine.setUpdatedAt(query.value("machine_updated_at").toDateTime());
                        machinesMap[machineId] = machine;
                        result.hierarchy.machinesByArea[areaId].append(machine);
                    }

                    if (!query.value("comp_id").isNull()) {
                        Competency competency;
                        competency.setId(query.value("comp_id").toInt());
                        competency.setMachineId(machineId);
                        competency.setName(query.value("comp_name").toString());
                        competency.setMaxScore(query.value("comp_max_score").toInt());
                        competency.setSafetyImpact(query.value("comp_safety_impact").toDouble());
                        competency.setProductionImpact(query.value("comp_production_impact").toDouble());
                        competency.setFrequency(query.value("comp_frequency").toDouble());
                        competency.setComplexity(query.value("comp_complexity").toDouble());
                        competency.setFutureValue(query.value("comp_future_value").toDouble());
                        competency.setCreatedAt(query.value("comp_created_at").toDateTime());
                        competency.setUpdatedAt(query.value("comp_updated_at").toDateTime());
                        result.hierarchy.competenciesByMachine[machineId].append(competency);
                    }
                }
            }

            result.hierarchy.areas = areasMap.values();

            workerDb.close();
            workerDb = QSqlDatabase();
        }

        QSqlDatabase::removeDatabase(connName);
        result.success = true;
        return result;
    });

    loadWatcher_->setFuture(future);
}

void DataCache::refresh()
{
    Logger::instance().info("DataCache", "Refreshing cache...");
    load();
}

void DataCache::invalidate()
{
    Logger::instance().info("DataCache", "Invalidating cache...");
    ++expectedAsyncRequestId_;
    isLoaded_ = false;
    isLoading_ = false;
    hierarchy_.areas.clear();
    hierarchy_.machinesByArea.clear();
    hierarchy_.competenciesByMachine.clear();
    areasById_.clear();
    machinesById_.clear();
    competenciesById_.clear();
    emit cacheInvalidated();
}

QList<ProductionArea> DataCache::getAreas() const
{
    return hierarchy_.areas;
}

QList<Machine> DataCache::getMachinesByArea(int areaId) const
{
    if (hierarchy_.machinesByArea.contains(areaId)) {
        return hierarchy_.machinesByArea[areaId];
    }
    return QList<Machine>();
}

QList<Competency> DataCache::getCompetenciesByMachine(int machineId) const
{
    if (hierarchy_.competenciesByMachine.contains(machineId)) {
        return hierarchy_.competenciesByMachine[machineId];
    }
    return QList<Competency>();
}

ProductionArea DataCache::getAreaById(int areaId) const
{
    if (areasById_.contains(areaId)) {
        return areasById_[areaId];
    }
    return ProductionArea();
}

Machine DataCache::getMachineById(int machineId) const
{
    if (machinesById_.contains(machineId)) {
        return machinesById_[machineId];
    }
    return Machine();
}

Competency DataCache::getCompetencyById(int competencyId) const
{
    if (competenciesById_.contains(competencyId)) {
        return competenciesById_[competencyId];
    }
    return Competency();
}

int DataCache::getTotalMachines() const
{
    return machinesById_.size();
}

int DataCache::getTotalCompetencies() const
{
    return competenciesById_.size();
}
