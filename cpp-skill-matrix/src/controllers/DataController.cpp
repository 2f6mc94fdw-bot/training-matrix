#include "DataController.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
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

    // Get all engineers and competencies
    QList<Engineer> engineers = engineerRepo.findAll();
    QList<ProductionArea> areas = productionRepo.findAllAreas();

    if (engineers.isEmpty()) {
        return "Error: No engineers found. Please add engineers first.";
    }

    // Collect all competencies
    QList<Competency> allCompetencies;
    for (const ProductionArea& area : areas) {
        QList<Machine> machines = productionRepo.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> competencies = productionRepo.findCompetenciesByMachine(machine.id());
            allCompetencies.append(competencies);
        }
    }

    if (allCompetencies.isEmpty()) {
        return "Error: No competencies found. Please add production areas and machines first.";
    }

    // Generate random assessments
    int totalCreated = 0;
    int totalSkipped = 0;
    QRandomGenerator* random = QRandomGenerator::global();
    QDateTime now = QDateTime::currentDateTime();

    for (const Engineer& engineer : engineers) {
        for (const Competency& competency : allCompetencies) {
            // Use percentage to determine if we should create an assessment for this combination
            if (random->bounded(100) < percentageToFill) {
                // Check if assessment already exists
                QList<Assessment> existing = assessmentRepo.findByEngineerAndCompetency(engineer.id(), competency.id());

                if (existing.isEmpty()) {
                    // Generate random score (0-3)
                    int score = random->bounded(competency.maxScore() + 1);

                    Assessment assessment(
                        0,  // id (auto-generated)
                        engineer.id(),
                        competency.id(),
                        score,
                        now,
                        "admin"  // assessor
                    );

                    if (assessmentRepo.save(assessment)) {
                        totalCreated++;
                    }
                } else {
                    totalSkipped++;
                }
            }
        }
    }

    QString message = QString("Successfully generated %1 random assessments for %2 engineers across %3 competencies.\n"
                             "%4 existing assessments were skipped.")
                          .arg(totalCreated)
                          .arg(engineers.size())
                          .arg(allCompetencies.size())
                          .arg(totalSkipped);

    Logger::instance().info("DataController", message);
    return message;
}
