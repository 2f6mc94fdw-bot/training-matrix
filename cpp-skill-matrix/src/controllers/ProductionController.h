#ifndef PRODUCTIONCONTROLLER_H
#define PRODUCTIONCONTROLLER_H

#include "../models/ProductionArea.h"
#include "../models/Machine.h"
#include "../models/Competency.h"
#include <QList>
#include <QString>

/**
 * @brief Controller for Production Area/Machine/Competency business logic
 *
 * Manages the hierarchical structure: Production Area -> Machines -> Competencies
 */
class ProductionController
{
public:
    ProductionController();
    ~ProductionController();

    // Production Areas
    QList<ProductionArea> getAllProductionAreas();
    ProductionArea getProductionAreaById(int id);
    int createProductionArea(const QString& name);
    bool updateProductionArea(int id, const QString& name);
    bool deleteProductionArea(int id);

    // Machines
    QList<Machine> getAllMachines();
    QList<Machine> getMachinesByProductionArea(int productionAreaId);
    Machine getMachineById(int id);
    int createMachine(int productionAreaId, const QString& name, int importance = 1);
    bool updateMachine(int id, int productionAreaId, const QString& name, int importance);
    bool deleteMachine(int id);

    // Competencies
    QList<Competency> getAllCompetencies();
    QList<Competency> getCompetenciesByMachine(int machineId);
    QList<Competency> getCompetenciesByProductionArea(int productionAreaId);
    Competency getCompetencyById(int id);
    int createCompetency(int machineId, const QString& name, int maxScore = 3);
    bool updateCompetency(int id, int machineId, const QString& name, int maxScore);
    bool deleteCompetency(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
    bool validateProductionArea(const QString& name);
    bool validateMachine(const QString& name, int importance);
    bool validateCompetency(const QString& name, int maxScore);
};

#endif // PRODUCTIONCONTROLLER_H
