#ifndef PRODUCTIONREPOSITORY_H
#define PRODUCTIONREPOSITORY_H

#include "../models/ProductionArea.h"
#include "../models/Machine.h"
#include "../models/Competency.h"
#include <QList>

class ProductionRepository
{
public:
    ProductionRepository();
    ~ProductionRepository();

    // Production Areas
    QList<ProductionArea> findAllAreas();
    ProductionArea findAreaById(int id);
    bool saveArea(ProductionArea& area);
    bool updateArea(const ProductionArea& area);
    bool removeArea(int id);

    // Machines
    QList<Machine> findMachinesByArea(int areaId);
    Machine findMachineById(int id);
    bool saveMachine(Machine& machine);
    bool updateMachine(const Machine& machine);
    bool removeMachine(int id);

    // Competencies
    QList<Competency> findCompetenciesByMachine(int machineId);
    Competency findCompetencyById(int id);
    bool saveCompetency(Competency& competency);
    bool updateCompetency(const Competency& competency);
    bool removeCompetency(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // PRODUCTIONREPOSITORY_H
