#include "DataController.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../models/Machine.h"
#include "../models/Competency.h"
#include "../utils/Logger.h"
#include <QRandomGenerator>
#include <QDateTime>

DataController::DataController() {}
DataController::~DataController() {}

QString DataController::generateRandomAssessments(int percentageToFill)
{
    EngineerRepository engineerRepo;
    ProductionRepository productionRepo;
    AssessmentRepository assessmentRepo;

    // Get all engineers
    QList<Engineer> engineers = engineerRepo.findAll();
    if (engineers.isEmpty()) {
        return "Error: No engineers found. Please add engineers first.";
    }

    // Collect all competencies with their area and machine IDs
    struct CompetencyInfo {
        Competency competency;
        int machineId;
        int areaId;
    };
    QList<CompetencyInfo> competencyInfos;

    QList<ProductionArea> areas = productionRepo.findAllAreas();
    for (const ProductionArea& area : areas) {
        QList<Machine> machines = productionRepo.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> competencies = productionRepo.findCompetenciesByMachine(machine.id());
            for (const Competency& competency : competencies) {
                CompetencyInfo info;
                info.competency = competency;
                info.machineId = machine.id();
                info.areaId = area.id();
                competencyInfos.append(info);
            }
        }
    }

    if (competencyInfos.isEmpty()) {
        return "Error: No competencies found. Please add production areas and machines first.";
    }

    // Generate random assessments
    int totalCreated = 0;
    int totalSkipped = 0;
    QRandomGenerator* random = QRandomGenerator::global();

    for (const Engineer& engineer : engineers) {
        // Get existing assessments for this engineer
        QList<Assessment> existingAssessments = assessmentRepo.findByEngineer(engineer.id());

        for (const CompetencyInfo& info : competencyInfos) {
            // Use percentage to determine if we should create an assessment
            if (random->bounded(100) < percentageToFill) {
                // Check if assessment already exists for this competency
                bool exists = false;
                for (const Assessment& existing : existingAssessments) {
                    if (existing.competencyId() == info.competency.id()) {
                        exists = true;
                        totalSkipped++;
                        break;
                    }
                }

                if (!exists) {
                    // Generate random score (0 to maxScore)
                    int score = random->bounded(info.competency.maxScore() + 1);

                    Assessment assessment(
                        0,  // id (auto-generated)
                        engineer.id(),
                        info.areaId,
                        info.machineId,
                        info.competency.id(),
                        score
                    );

                    if (assessmentRepo.saveOrUpdate(assessment)) {
                        totalCreated++;
                    }
                }
            }
        }
    }

    QString message = QString("Successfully generated %1 random assessments for %2 engineers across %3 competencies.\n"
                             "%4 existing assessments were skipped.")
                          .arg(totalCreated)
                          .arg(engineers.size())
                          .arg(competencyInfos.size())
                          .arg(totalSkipped);

    Logger::instance().info("DataController", message);
    return message;
}
