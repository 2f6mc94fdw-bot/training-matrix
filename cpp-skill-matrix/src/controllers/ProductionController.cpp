#include "ProductionController.h"
#include "../database/ProductionRepository.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"

ProductionController::ProductionController() : lastError_("") {}
ProductionController::~ProductionController() {}

// Production Areas
QList<ProductionArea> ProductionController::getAllProductionAreas()
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findAllAreas();
}

ProductionArea ProductionController::getProductionAreaById(int id)
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findAreaById(id);
}

int ProductionController::createProductionArea(const QString& name)
{
    lastError_.clear();

    if (!validateProductionArea(name)) {
        return -1;
    }

    ProductionArea area;
    area.setName(name);

    ProductionRepository repo;
    bool success = repo.saveArea(area);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("ProductionController", "Failed to create production area: " + lastError_);
        return -1;
    }

    Logger::instance().info("ProductionController", "Created production area: " + name);
    return area.id();
}

bool ProductionController::updateProductionArea(int id, const QString& name)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Invalid production area ID";
        return false;
    }

    if (!validateProductionArea(name)) {
        return false;
    }

    ProductionRepository repo;
    ProductionArea area = repo.findAreaById(id);

    if (!area.isValid()) {
        lastError_ = "Production area not found: " + QString::number(id);
        return false;
    }

    area.setName(name);
    bool success = repo.updateArea(area);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

bool ProductionController::deleteProductionArea(int id)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Invalid production area ID";
        return false;
    }

    ProductionRepository repo;
    bool success = repo.removeArea(id);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

// Machines
QList<Machine> ProductionController::getAllMachines()
{
    lastError_.clear();
    ProductionRepository repo;

    // Get all machines by iterating through all areas
    QList<Machine> allMachines;
    QList<ProductionArea> areas = repo.findAllAreas();

    for (const ProductionArea& area : areas) {
        QList<Machine> areaMachines = repo.findMachinesByArea(area.id());
        allMachines.append(areaMachines);
    }

    return allMachines;
}

QList<Machine> ProductionController::getMachinesByProductionArea(int productionAreaId)
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findMachinesByArea(productionAreaId);
}

Machine ProductionController::getMachineById(int id)
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findMachineById(id);
}

int ProductionController::createMachine(int productionAreaId, const QString& name, int importance)
{
    lastError_.clear();

    if (productionAreaId <= 0) {
        lastError_ = "Invalid production area ID";
        return -1;
    }

    if (!validateMachine(name, importance)) {
        return -1;
    }

    Machine machine;
    machine.setProductionAreaId(productionAreaId);
    machine.setName(name);
    machine.setImportance(importance);

    ProductionRepository repo;
    bool success = repo.saveMachine(machine);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("ProductionController", "Failed to create machine: " + lastError_);
        return -1;
    }

    Logger::instance().info("ProductionController", "Created machine: " + name);
    return machine.id();
}

bool ProductionController::updateMachine(int id, int productionAreaId, const QString& name, int importance)
{
    lastError_.clear();

    if (id <= 0 || productionAreaId <= 0) {
        lastError_ = "Invalid machine or production area ID";
        return false;
    }

    if (!validateMachine(name, importance)) {
        return false;
    }

    ProductionRepository repo;
    Machine machine = repo.findMachineById(id);

    if (!machine.isValid()) {
        lastError_ = "Machine not found: " + QString::number(id);
        return false;
    }

    machine.setProductionAreaId(productionAreaId);
    machine.setName(name);
    machine.setImportance(importance);

    bool success = repo.updateMachine(machine);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

bool ProductionController::deleteMachine(int id)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Invalid machine ID";
        return false;
    }

    ProductionRepository repo;
    bool success = repo.removeMachine(id);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

// Competencies
QList<Competency> ProductionController::getAllCompetencies()
{
    lastError_.clear();
    ProductionRepository repo;

    // Get all competencies by iterating through all machines
    QList<Competency> allCompetencies;
    QList<Machine> machines = getAllMachines();

    for (const Machine& machine : machines) {
        QList<Competency> machineComps = repo.findCompetenciesByMachine(machine.id());
        allCompetencies.append(machineComps);
    }

    return allCompetencies;
}

QList<Competency> ProductionController::getCompetenciesByMachine(int machineId)
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findCompetenciesByMachine(machineId);
}

QList<Competency> ProductionController::getCompetenciesByProductionArea(int productionAreaId)
{
    lastError_.clear();
    ProductionRepository repo;

    // Get competencies by iterating through machines in this area
    QList<Competency> areaCompetencies;
    QList<Machine> machines = repo.findMachinesByArea(productionAreaId);

    for (const Machine& machine : machines) {
        QList<Competency> machineComps = repo.findCompetenciesByMachine(machine.id());
        areaCompetencies.append(machineComps);
    }

    return areaCompetencies;
}

Competency ProductionController::getCompetencyById(int id)
{
    lastError_.clear();
    ProductionRepository repo;
    return repo.findCompetencyById(id);
}

int ProductionController::createCompetency(int machineId, const QString& name, int maxScore)
{
    lastError_.clear();

    if (machineId <= 0) {
        lastError_ = "Invalid machine ID";
        return -1;
    }

    if (!validateCompetency(name, maxScore)) {
        return -1;
    }

    Competency competency;
    competency.setMachineId(machineId);
    competency.setName(name);
    competency.setMaxScore(maxScore);

    ProductionRepository repo;
    bool success = repo.saveCompetency(competency);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("ProductionController", "Failed to create competency: " + lastError_);
        return -1;
    }

    Logger::instance().info("ProductionController", "Created competency: " + name);
    return competency.id();
}

bool ProductionController::updateCompetency(int id, int machineId, const QString& name, int maxScore)
{
    lastError_.clear();

    if (id <= 0 || machineId <= 0) {
        lastError_ = "Invalid competency or machine ID";
        return false;
    }

    if (!validateCompetency(name, maxScore)) {
        return false;
    }

    ProductionRepository repo;
    Competency competency = repo.findCompetencyById(id);

    if (!competency.isValid()) {
        lastError_ = "Competency not found: " + QString::number(id);
        return false;
    }

    competency.setMachineId(machineId);
    competency.setName(name);
    competency.setMaxScore(maxScore);

    bool success = repo.updateCompetency(competency);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

bool ProductionController::deleteCompetency(int id)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Invalid competency ID";
        return false;
    }

    ProductionRepository repo;
    bool success = repo.removeCompetency(id);

    if (!success) {
        lastError_ = repo.lastError();
    }

    return success;
}

// Validation methods
bool ProductionController::validateProductionArea(const QString& name)
{
    if (name.trimmed().isEmpty()) {
        lastError_ = "Production area name cannot be empty";
        return false;
    }

    if (name.length() > 200) {
        lastError_ = "Production area name too long (max 200 characters)";
        return false;
    }

    return true;
}

bool ProductionController::validateMachine(const QString& name, int importance)
{
    if (name.trimmed().isEmpty()) {
        lastError_ = "Machine name cannot be empty";
        return false;
    }

    if (name.length() > 200) {
        lastError_ = "Machine name too long (max 200 characters)";
        return false;
    }

    if (importance < 1 || importance > 5) {
        lastError_ = "Machine importance must be between 1 and 5";
        return false;
    }

    return true;
}

bool ProductionController::validateCompetency(const QString& name, int maxScore)
{
    if (name.trimmed().isEmpty()) {
        lastError_ = "Competency name cannot be empty";
        return false;
    }

    if (name.length() > 200) {
        lastError_ = "Competency name too long (max 200 characters)";
        return false;
    }

    if (maxScore < 1 || maxScore > Constants::SCORE_MAX) {
        lastError_ = QString("Max score must be between 1 and %1").arg(Constants::SCORE_MAX);
        return false;
    }

    return true;
}
