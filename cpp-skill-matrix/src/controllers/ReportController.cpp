#include "ReportController.h"
#include "CoreSkillsController.h"
#include "EngineerController.h"
#include "AssessmentController.h"
#include "ProductionController.h"
#include "../utils/Logger.h"

ReportController::ReportController() : lastError_("") {}
ReportController::~ReportController() {}

QMap<QString, QString> ReportController::generateEngineerReport(const QString& engineerId)
{
    lastError_.clear();
    QMap<QString, QString> report;

    EngineerController engineerCtrl;
    auto engineer = engineerCtrl.getEngineerById(engineerId);

    if (!engineer.isValid()) {
        lastError_ = "Engineer not found: " + engineerId;
        return report;
    }

    CoreSkillsController coreSkillsCtrl;
    auto coreSkills = coreSkillsCtrl.getAllSkills();
    auto coreAssessments = coreSkillsCtrl.getEngineerAssessments(engineerId);

    AssessmentController assessmentCtrl;
    auto machineAssessments = assessmentCtrl.getAssessmentsByEngineer(engineerId);

    int assessedCoreSkills = 0;
    int totalCoreScore = 0;
    for (const auto& assessment : coreAssessments) {
        if (assessment.score() > 0) {
            assessedCoreSkills++;
            totalCoreScore += assessment.score();
        }
    }

    report["engineer_id"] = engineerId;
    report["engineer_name"] = engineer.name();
    report["shift"] = engineer.shift();
    report["total_core_skills"] = QString::number(coreSkills.size());
    report["assessed_core_skills"] = QString::number(assessedCoreSkills);
    report["core_completion_percentage"] = QString::number(coreSkillsCtrl.getCompletionPercentage(engineerId), 'f', 1);
    report["core_average_score"] = QString::number(coreSkillsCtrl.getAverageScore(engineerId), 'f', 2);
    report["total_machine_assessments"] = QString::number(machineAssessments.size());

    return report;
}

QMap<QString, QString> ReportController::generateProductionAreaReport(int productionAreaId)
{
    lastError_.clear();
    QMap<QString, QString> report;

    ProductionController prodCtrl;
    auto area = prodCtrl.getProductionAreaById(productionAreaId);

    if (!area.isValid()) {
        lastError_ = "Production area not found: " + QString::number(productionAreaId);
        return report;
    }

    auto machines = prodCtrl.getMachinesByProductionArea(productionAreaId);
    auto competencies = prodCtrl.getCompetenciesByProductionArea(productionAreaId);

    AssessmentController assessmentCtrl;
    auto assessments = assessmentCtrl.getAssessmentsByProductionArea(productionAreaId);

    int assessedCompetencies = 0;
    for (const auto& assessment : assessments) {
        if (assessment.score() > 0) {
            assessedCompetencies++;
        }
    }

    double coverage = competencies.size() > 0 ?
        (double)assessedCompetencies / competencies.size() * 100.0 : 0.0;

    report["area_id"] = QString::number(productionAreaId);
    report["area_name"] = area.name();
    report["machine_count"] = QString::number(machines.size());
    report["total_competencies"] = QString::number(competencies.size());
    report["assessed_competencies"] = QString::number(assessedCompetencies);
    report["coverage_percentage"] = QString::number(coverage, 'f', 1);
    report["total_assessments"] = QString::number(assessments.size());

    return report;
}

QMap<QString, QString> ReportController::generateShiftReport(const QString& shift)
{
    lastError_.clear();
    QMap<QString, QString> report;

    EngineerController engineerCtrl;
    auto engineers = engineerCtrl.getEngineersByShift(shift);

    CoreSkillsController coreSkillsCtrl;
    AssessmentController assessmentCtrl;

    int totalCoreAssessments = 0;
    int totalMachineAssessments = 0;
    double totalCoreAvg = 0.0;

    for (const auto& engineer : engineers) {
        auto coreAssessments = coreSkillsCtrl.getEngineerAssessments(engineer.id());
        auto machineAssessments = assessmentCtrl.getAssessmentsByEngineer(engineer.id());

        totalCoreAssessments += coreAssessments.size();
        totalMachineAssessments += machineAssessments.size();
        totalCoreAvg += coreSkillsCtrl.getAverageScore(engineer.id());
    }

    double avgCoreScore = engineers.size() > 0 ? totalCoreAvg / engineers.size() : 0.0;

    report["shift"] = shift;
    report["engineer_count"] = QString::number(engineers.size());
    report["total_core_assessments"] = QString::number(totalCoreAssessments);
    report["total_machine_assessments"] = QString::number(totalMachineAssessments);
    report["average_core_score"] = QString::number(avgCoreScore, 'f', 2);

    return report;
}

QMap<QString, QString> ReportController::generateSystemReport()
{
    lastError_.clear();
    QMap<QString, QString> report;

    EngineerController engineerCtrl;
    auto engineers = engineerCtrl.getAllEngineers();

    ProductionController prodCtrl;
    auto areas = prodCtrl.getAllProductionAreas();
    auto machines = prodCtrl.getAllMachines();
    auto competencies = prodCtrl.getAllCompetencies();

    CoreSkillsController coreSkillsCtrl;
    auto coreSkills = coreSkillsCtrl.getAllSkills();
    auto coreAssessments = coreSkillsCtrl.getAllSkills(); // All from repo

    AssessmentController assessmentCtrl;
    auto machineAssessments = assessmentCtrl.getAllAssessments();

    report["total_engineers"] = QString::number(engineers.size());
    report["total_production_areas"] = QString::number(areas.size());
    report["total_machines"] = QString::number(machines.size());
    report["total_competencies"] = QString::number(competencies.size());
    report["total_core_skills"] = QString::number(coreSkills.size());
    report["total_machine_assessments"] = QString::number(machineAssessments.size());

    return report;
}
