#include "ProductionRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

ProductionRepository::ProductionRepository() : lastError_("") {}
ProductionRepository::~ProductionRepository() {}

// ============================================================================
// Production Areas
// ============================================================================

QList<ProductionArea> ProductionRepository::findAllAreas()
{
    QList<ProductionArea> areas;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return areas;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, created_at, updated_at FROM production_areas ORDER BY name");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findAllAreas failed: " + lastError_);
        return areas;
    }

    while (query.next()) {
        ProductionArea area;
        area.setId(query.value(0).toInt());
        area.setName(query.value(1).toString());
        area.setCreatedAt(query.value(2).toDateTime());
        area.setUpdatedAt(query.value(3).toDateTime());
        areas.append(area);
    }

    Logger::instance().debug("ProductionRepository", QString("Found %1 production areas").arg(areas.size()));
    return areas;
}

ProductionArea ProductionRepository::findAreaById(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return ProductionArea();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, created_at, updated_at FROM production_areas WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findAreaById failed: " + lastError_);
        return ProductionArea();
    }

    if (query.next()) {
        ProductionArea area;
        area.setId(query.value(0).toInt());
        area.setName(query.value(1).toString());
        area.setCreatedAt(query.value(2).toDateTime());
        area.setUpdatedAt(query.value(3).toDateTime());

        Logger::instance().debug("ProductionRepository", "Found production area: " + area.name());
        return area;
    }

    Logger::instance().debug("ProductionRepository", QString("Production area not found with id: %1").arg(id));
    return ProductionArea();
}

bool ProductionRepository::saveArea(ProductionArea& area)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO production_areas (name, created_at, updated_at) "
                  "VALUES (?, GETDATE(), GETDATE()); "
                  "SELECT SCOPE_IDENTITY();");
    query.addBindValue(area.name());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "saveArea failed: " + lastError_);
        return false;
    }

    // Get the generated ID
    if (query.next()) {
        int newId = query.value(0).toInt();
        area.setId(newId);
        Logger::instance().info("ProductionRepository", QString("Production area saved: %1 (ID: %2)").arg(area.name()).arg(newId));
        return true;
    }

    lastError_ = "Failed to retrieve generated ID";
    Logger::instance().error("ProductionRepository", lastError_);
    return false;
}

bool ProductionRepository::updateArea(const ProductionArea& area)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE production_areas SET name = ?, updated_at = GETDATE() WHERE id = ?");
    query.addBindValue(area.name());
    query.addBindValue(area.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "updateArea failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Production area updated: %1").arg(area.name()));
    return true;
}

bool ProductionRepository::removeArea(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM production_areas WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "removeArea failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Production area removed: %1").arg(id));
    return true;
}

// ============================================================================
// Machines
// ============================================================================

QList<Machine> ProductionRepository::findMachinesByArea(int areaId)
{
    QList<Machine> machines;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return machines;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, production_area_id, name, importance, created_at, updated_at "
                  "FROM machines WHERE production_area_id = ? ORDER BY name");
    query.addBindValue(areaId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findMachinesByArea failed: " + lastError_);
        return machines;
    }

    while (query.next()) {
        Machine machine;
        machine.setId(query.value(0).toInt());
        machine.setProductionAreaId(query.value(1).toInt());
        machine.setName(query.value(2).toString());
        machine.setImportance(query.value(3).toInt());
        machine.setCreatedAt(query.value(4).toDateTime());
        machine.setUpdatedAt(query.value(5).toDateTime());
        machines.append(machine);
    }

    Logger::instance().debug("ProductionRepository", QString("Found %1 machines for area %2").arg(machines.size()).arg(areaId));
    return machines;
}

Machine ProductionRepository::findMachineById(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return Machine();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, production_area_id, name, importance, created_at, updated_at "
                  "FROM machines WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findMachineById failed: " + lastError_);
        return Machine();
    }

    if (query.next()) {
        Machine machine;
        machine.setId(query.value(0).toInt());
        machine.setProductionAreaId(query.value(1).toInt());
        machine.setName(query.value(2).toString());
        machine.setImportance(query.value(3).toInt());
        machine.setCreatedAt(query.value(4).toDateTime());
        machine.setUpdatedAt(query.value(5).toDateTime());

        Logger::instance().debug("ProductionRepository", "Found machine: " + machine.name());
        return machine;
    }

    Logger::instance().debug("ProductionRepository", QString("Machine not found with id: %1").arg(id));
    return Machine();
}

bool ProductionRepository::saveMachine(Machine& machine)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO machines (production_area_id, name, importance, created_at, updated_at) "
                  "VALUES (?, ?, ?, GETDATE(), GETDATE()); "
                  "SELECT SCOPE_IDENTITY();");
    query.addBindValue(machine.productionAreaId());
    query.addBindValue(machine.name());
    query.addBindValue(machine.importance());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "saveMachine failed: " + lastError_);
        return false;
    }

    // Get the generated ID
    if (query.next()) {
        int newId = query.value(0).toInt();
        machine.setId(newId);
        Logger::instance().info("ProductionRepository", QString("Machine saved: %1 (ID: %2)").arg(machine.name()).arg(newId));
        return true;
    }

    lastError_ = "Failed to retrieve generated ID";
    Logger::instance().error("ProductionRepository", lastError_);
    return false;
}

bool ProductionRepository::updateMachine(const Machine& machine)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE machines SET name = ?, importance = ?, updated_at = GETDATE() WHERE id = ?");
    query.addBindValue(machine.name());
    query.addBindValue(machine.importance());
    query.addBindValue(machine.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "updateMachine failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Machine updated: %1").arg(machine.name()));
    return true;
}

bool ProductionRepository::removeMachine(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM machines WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "removeMachine failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Machine removed: %1").arg(id));
    return true;
}

// ============================================================================
// Competencies
// ============================================================================

QList<Competency> ProductionRepository::findCompetenciesByMachine(int machineId)
{
    QList<Competency> competencies;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return competencies;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, machine_id, name, max_score, created_at, updated_at, "
                  "safety_impact, production_impact, frequency, complexity, future_value "
                  "FROM competencies WHERE machine_id = ? ORDER BY name");
    query.addBindValue(machineId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findCompetenciesByMachine failed: " + lastError_);
        return competencies;
    }

    while (query.next()) {
        Competency competency;
        competency.setId(query.value(0).toInt());
        competency.setMachineId(query.value(1).toInt());
        competency.setName(query.value(2).toString());
        competency.setMaxScore(query.value(3).toInt());
        competency.setCreatedAt(query.value(4).toDateTime());
        competency.setUpdatedAt(query.value(5).toDateTime());

        // Multi-Criteria Weighting
        competency.setSafetyImpact(query.value(6).toDouble());
        competency.setProductionImpact(query.value(7).toDouble());
        competency.setFrequency(query.value(8).toDouble());
        competency.setComplexity(query.value(9).toDouble());
        competency.setFutureValue(query.value(10).toDouble());

        competencies.append(competency);
    }

    Logger::instance().debug("ProductionRepository", QString("Found %1 competencies for machine %2").arg(competencies.size()).arg(machineId));
    return competencies;
}

Competency ProductionRepository::findCompetencyById(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return Competency();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, machine_id, name, max_score, created_at, updated_at, "
                  "safety_impact, production_impact, frequency, complexity, future_value "
                  "FROM competencies WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "findCompetencyById failed: " + lastError_);
        return Competency();
    }

    if (query.next()) {
        Competency competency;
        competency.setId(query.value(0).toInt());
        competency.setMachineId(query.value(1).toInt());
        competency.setName(query.value(2).toString());
        competency.setMaxScore(query.value(3).toInt());
        competency.setCreatedAt(query.value(4).toDateTime());
        competency.setUpdatedAt(query.value(5).toDateTime());

        // Multi-Criteria Weighting
        competency.setSafetyImpact(query.value(6).toDouble());
        competency.setProductionImpact(query.value(7).toDouble());
        competency.setFrequency(query.value(8).toDouble());
        competency.setComplexity(query.value(9).toDouble());
        competency.setFutureValue(query.value(10).toDouble());

        Logger::instance().debug("ProductionRepository", "Found competency: " + competency.name());
        return competency;
    }

    Logger::instance().debug("ProductionRepository", QString("Competency not found with id: %1").arg(id));
    return Competency();
}

bool ProductionRepository::saveCompetency(Competency& competency)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO competencies (machine_id, name, max_score, "
                  "safety_impact, production_impact, frequency, complexity, future_value, "
                  "created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, GETDATE(), GETDATE()); "
                  "SELECT SCOPE_IDENTITY();");
    query.addBindValue(competency.machineId());
    query.addBindValue(competency.name());
    query.addBindValue(competency.maxScore());
    query.addBindValue(competency.safetyImpact());
    query.addBindValue(competency.productionImpact());
    query.addBindValue(competency.frequency());
    query.addBindValue(competency.complexity());
    query.addBindValue(competency.futureValue());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "saveCompetency failed: " + lastError_);
        return false;
    }

    // Get the generated ID
    if (query.next()) {
        int newId = query.value(0).toInt();
        competency.setId(newId);
        Logger::instance().info("ProductionRepository", QString("Competency saved: %1 (ID: %2)").arg(competency.name()).arg(newId));
        return true;
    }

    lastError_ = "Failed to retrieve generated ID";
    Logger::instance().error("ProductionRepository", lastError_);
    return false;
}

bool ProductionRepository::updateCompetency(const Competency& competency)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE competencies SET name = ?, max_score = ?, "
                  "safety_impact = ?, production_impact = ?, frequency = ?, "
                  "complexity = ?, future_value = ?, updated_at = GETDATE() WHERE id = ?");
    query.addBindValue(competency.name());
    query.addBindValue(competency.maxScore());
    query.addBindValue(competency.safetyImpact());
    query.addBindValue(competency.productionImpact());
    query.addBindValue(competency.frequency());
    query.addBindValue(competency.complexity());
    query.addBindValue(competency.futureValue());
    query.addBindValue(competency.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "updateCompetency failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Competency updated: %1").arg(competency.name()));
    return true;
}

bool ProductionRepository::removeCompetency(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("ProductionRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM competencies WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("ProductionRepository", "removeCompetency failed: " + lastError_);
        return false;
    }

    Logger::instance().info("ProductionRepository", QString("Competency removed: %1").arg(id));
    return true;
}
