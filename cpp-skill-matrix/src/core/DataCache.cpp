#include "DataCache.h"
#include "../utils/Logger.h"

DataCache::DataCache()
    : isLoaded_(false)
    , lastError_("")
{
}

DataCache::~DataCache()
{
}

DataCache& DataCache::instance()
{
    static DataCache instance;
    return instance;
}

void DataCache::load()
{
    Logger::instance().info("DataCache", "Loading production hierarchy...");
    emit loadStarted();

    isLoaded_ = false;
    lastError_ = "";

    // Load complete hierarchy in single query
    hierarchy_ = repository_.loadCompleteHierarchy();

    if (!repository_.lastError().isEmpty()) {
        lastError_ = repository_.lastError();
        Logger::instance().error("DataCache", "Failed to load hierarchy: " + lastError_);
        emit loadCompleted(false);
        return;
    }

    // Build quick lookup maps
    areasById_.clear();
    for (const ProductionArea& area : hierarchy_.areas) {
        areasById_[area.id()] = area;
    }

    machinesById_.clear();
    for (auto it = hierarchy_.machinesByArea.begin(); it != hierarchy_.machinesByArea.end(); ++it) {
        for (const Machine& machine : it.value()) {
            machinesById_[machine.id()] = machine;
        }
    }

    competenciesById_.clear();
    for (auto it = hierarchy_.competenciesByMachine.begin(); it != hierarchy_.competenciesByMachine.end(); ++it) {
        for (const Competency& competency : it.value()) {
            competenciesById_[competency.id()] = competency;
        }
    }

    isLoaded_ = true;
    lastLoadTime_ = QDateTime::currentDateTime();

    Logger::instance().info("DataCache",
        QString("Cache loaded successfully: %1 areas, %2 machines, %3 competencies")
            .arg(hierarchy_.areas.size())
            .arg(machinesById_.size())
            .arg(competenciesById_.size()));

    emit loadCompleted(true);
}

void DataCache::refresh()
{
    Logger::instance().info("DataCache", "Refreshing cache...");
    load();
}

void DataCache::invalidate()
{
    Logger::instance().info("DataCache", "Invalidating cache...");
    isLoaded_ = false;
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
