#include "ProductionRepository.h"
#include "../utils/Logger.h"

ProductionRepository::ProductionRepository() : lastError_("") {}
ProductionRepository::~ProductionRepository() {}

QList<ProductionArea> ProductionRepository::findAllAreas() { return QList<ProductionArea>(); }
ProductionArea ProductionRepository::findAreaById(int id) { return ProductionArea(); }
bool ProductionRepository::saveArea(ProductionArea& area) { return false; }
bool ProductionRepository::updateArea(const ProductionArea& area) { return false; }
bool ProductionRepository::removeArea(int id) { return false; }

QList<Machine> ProductionRepository::findMachinesByArea(int areaId) { return QList<Machine>(); }
Machine ProductionRepository::findMachineById(int id) { return Machine(); }
bool ProductionRepository::saveMachine(Machine& machine) { return false; }
bool ProductionRepository::updateMachine(const Machine& machine) { return false; }
bool ProductionRepository::removeMachine(int id) { return false; }

QList<Competency> ProductionRepository::findCompetenciesByMachine(int machineId) { return QList<Competency>(); }
Competency ProductionRepository::findCompetencyById(int id) { return Competency(); }
bool ProductionRepository::saveCompetency(Competency& competency) { return false; }
bool ProductionRepository::updateCompetency(const Competency& competency) { return false; }
bool ProductionRepository::removeCompetency(int id) { return false; }
