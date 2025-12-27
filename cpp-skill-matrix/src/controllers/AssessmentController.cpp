#include "AssessmentController.h"
#include "../database/AssessmentRepository.h"
#include "../database/ProductionRepository.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"

AssessmentController::AssessmentController() : lastError_("") {}
AssessmentController::~AssessmentController() {}

QList<Assessment> AssessmentController::getAllAssessments()
{
    lastError_.clear();
    AssessmentRepository repo;
    QList<Assessment> assessments = repo.findAll();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to get assessments: " + lastError_);
    }

    return assessments;
}

QList<Assessment> AssessmentController::getAssessmentsByEngineer(const QString& engineerId)
{
    lastError_.clear();
    AssessmentRepository repo;
    QList<Assessment> assessments = repo.findByEngineer(engineerId);

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to get assessments by engineer: " + lastError_);
    }

    return assessments;
}

QList<Assessment> AssessmentController::getAssessmentsByProductionArea(int productionAreaId)
{
    lastError_.clear();
    AssessmentRepository repo;
    QList<Assessment> allAssessments = repo.findAll();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to get assessments by production area: " + lastError_);
        return QList<Assessment>();
    }

    // Filter by production area ID
    QList<Assessment> filtered;
    for (const Assessment& assessment : allAssessments) {
        if (assessment.productionAreaId() == productionAreaId) {
            filtered.append(assessment);
        }
    }

    return filtered;
}

Assessment AssessmentController::getAssessment(const QString& engineerId, int competencyId)
{
    lastError_.clear();
    AssessmentRepository repo;
    QList<Assessment> assessments = repo.findByEngineer(engineerId);

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to get assessment: " + lastError_);
        return Assessment();
    }

    // Find assessment for this competency
    for (const Assessment& assessment : assessments) {
        if (assessment.competencyId() == competencyId) {
            return assessment;
        }
    }

    return Assessment(); // Not found
}

bool AssessmentController::updateAssessment(const QString& engineerId, int productionAreaId,
                                           int machineId, int competencyId, int score)
{
    lastError_.clear();

    // Validate score
    if (score < 0 || score > Constants::SCORE_MAX) {
        lastError_ = QString("Invalid score: %1. Must be between 0 and %2")
            .arg(score).arg(Constants::SCORE_MAX);
        Logger::instance().warning("AssessmentController", lastError_);
        return false;
    }

    // Validate engineer ID
    if (engineerId.isEmpty()) {
        lastError_ = "Engineer ID cannot be empty";
        Logger::instance().warning("AssessmentController", lastError_);
        return false;
    }

    // Validate IDs
    if (productionAreaId <= 0 || machineId <= 0 || competencyId <= 0) {
        lastError_ = "Production area, machine, and competency IDs must be positive integers";
        Logger::instance().warning("AssessmentController", lastError_);
        return false;
    }

    // Create assessment object
    Assessment assessment;
    assessment.setEngineerId(engineerId);
    assessment.setProductionAreaId(productionAreaId);
    assessment.setMachineId(machineId);
    assessment.setCompetencyId(competencyId);
    assessment.setScore(score);

    // Save to repository
    AssessmentRepository repo;
    bool success = repo.saveOrUpdate(assessment);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to update assessment: " + lastError_);
    }

    return success;
}

bool AssessmentController::deleteAssessment(int id)
{
    lastError_.clear();

    if (id <= 0) {
        lastError_ = "Assessment ID must be a positive integer";
        Logger::instance().warning("AssessmentController", lastError_);
        return false;
    }

    AssessmentRepository repo;
    bool success = repo.remove(id);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("AssessmentController", "Failed to delete assessment: " + lastError_);
    } else {
        Logger::instance().info("AssessmentController", QString("Deleted assessment: %1").arg(id));
    }

    return success;
}

double AssessmentController::getCompletionPercentage(const QString& engineerId, int productionAreaId)
{
    lastError_.clear();

    // Get all competencies in the production area by iterating through machines
    ProductionRepository prodRepo;
    QList<Machine> machines = prodRepo.findMachinesByArea(productionAreaId);

    QList<Competency> competencies;
    for (const Machine& machine : machines) {
        QList<Competency> machineComps = prodRepo.findCompetenciesByMachine(machine.id());
        competencies.append(machineComps);
    }

    int totalCompetencies = competencies.size();
    if (totalCompetencies == 0) {
        return 0.0;
    }

    // Get engineer's assessments in this production area
    QList<Assessment> areaAssessments = getAssessmentsByProductionArea(productionAreaId);

    // Count assessed competencies for this engineer
    int assessedCount = 0;
    for (const Assessment& assessment : areaAssessments) {
        if (assessment.engineerId() == engineerId && assessment.score() > 0) {
            assessedCount++;
        }
    }

    return (double)assessedCount / totalCompetencies * 100.0;
}
