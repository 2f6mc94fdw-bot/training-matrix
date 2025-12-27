#include "AnalyticsController.h"
#include "CoreSkillsController.h"
#include "EngineerController.h"
#include "AssessmentController.h"
#include "CertificationController.h"
#include "ProductionController.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"

AnalyticsController::AnalyticsController() : lastError_("") {}
AnalyticsController::~AnalyticsController() {}

QMap<QString, int> AnalyticsController::getSkillDistribution()
{
    lastError_.clear();
    QMap<QString, int> distribution;

    // Initialize distribution
    distribution["not_assessed"] = 0;
    distribution["basic"] = 0;
    distribution["intermediate"] = 0;
    distribution["advanced"] = 0;

    CoreSkillsController coreCtrl;
    auto allSkills = coreCtrl.getAllSkills();

    EngineerController engineerCtrl;
    auto engineers = engineerCtrl.getAllEngineers();

    for (const auto& engineer : engineers) {
        auto assessments = coreCtrl.getEngineerAssessments(engineer.id());

        // Count assessed skills
        QMap<QString, bool> assessedSkillIds;
        for (const auto& assessment : assessments) {
            assessedSkillIds[assessment.skillId()] = true;

            switch (assessment.score()) {
                case 0: distribution["not_assessed"]++; break;
                case 1: distribution["basic"]++; break;
                case 2: distribution["intermediate"]++; break;
                case 3: distribution["advanced"]++; break;
            }
        }

        // Count not assessed skills
        for (const auto& skill : allSkills) {
            if (!assessedSkillIds.contains(skill.id())) {
                distribution["not_assessed"]++;
            }
        }
    }

    return distribution;
}

QMap<QString, double> AnalyticsController::getCompletionRatesByShift()
{
    lastError_.clear();
    QMap<QString, double> rates;

    QStringList shifts = {
        Constants::SHIFT_A,
        Constants::SHIFT_B,
        Constants::SHIFT_C,
        Constants::SHIFT_D,
        Constants::SHIFT_DAY
    };

    EngineerController engineerCtrl;
    CoreSkillsController coreCtrl;

    for (const QString& shift : shifts) {
        auto engineers = engineerCtrl.getEngineersByShift(shift);

        if (engineers.isEmpty()) {
            rates[shift] = 0.0;
            continue;
        }

        double totalCompletion = 0.0;
        for (const auto& engineer : engineers) {
            totalCompletion += coreCtrl.getCompletionPercentage(engineer.id());
        }

        rates[shift] = totalCompletion / engineers.size();
    }

    return rates;
}

QList<QString> AnalyticsController::getTopPerformers(int limit)
{
    lastError_.clear();
    QList<QString> topPerformers;

    EngineerController engineerCtrl;
    CoreSkillsController coreCtrl;

    auto engineers = engineerCtrl.getAllEngineers();

    // Create list of (engineerId, avgScore) pairs
    QList<QPair<QString, double>> engineerScores;

    for (const auto& engineer : engineers) {
        double avgScore = coreCtrl.getAverageScore(engineer.id());
        if (avgScore > 0.0) {  // Only include engineers with assessments
            engineerScores.append(qMakePair(engineer.id(), avgScore));
        }
    }

    // Sort by score descending
    std::sort(engineerScores.begin(), engineerScores.end(),
              [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                  return a.second > b.second;
              });

    // Get top N
    int count = qMin(limit, engineerScores.size());
    for (int i = 0; i < count; i++) {
        topPerformers.append(engineerScores[i].first);
    }

    return topPerformers;
}

QList<QString> AnalyticsController::getEngineersNeedingImprovement(int limit)
{
    lastError_.clear();
    QList<QString> needingImprovement;

    EngineerController engineerCtrl;
    CoreSkillsController coreCtrl;

    auto engineers = engineerCtrl.getAllEngineers();

    // Create list of (engineerId, avgScore) pairs
    QList<QPair<QString, double>> engineerScores;

    for (const auto& engineer : engineers) {
        double avgScore = coreCtrl.getAverageScore(engineer.id());
        if (avgScore > 0.0) {  // Only include engineers with assessments
            engineerScores.append(qMakePair(engineer.id(), avgScore));
        }
    }

    // Sort by score ascending (lowest first)
    std::sort(engineerScores.begin(), engineerScores.end(),
              [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                  return a.second < b.second;
              });

    // Get bottom N
    int count = qMin(limit, engineerScores.size());
    for (int i = 0; i < count; i++) {
        needingImprovement.append(engineerScores[i].first);
    }

    return needingImprovement;
}

QMap<int, double> AnalyticsController::getProductionAreaCoverage()
{
    lastError_.clear();
    QMap<int, double> coverage;

    ProductionController prodCtrl;
    AssessmentController assessmentCtrl;

    auto areas = prodCtrl.getAllProductionAreas();

    for (const auto& area : areas) {
        auto competencies = prodCtrl.getCompetenciesByProductionArea(area.id());
        auto assessments = assessmentCtrl.getAssessmentsByProductionArea(area.id());

        int assessedCount = 0;
        for (const auto& assessment : assessments) {
            if (assessment.score() > 0) {
                assessedCount++;
            }
        }

        double coveragePercent = competencies.size() > 0 ?
            (double)assessedCount / competencies.size() * 100.0 : 0.0;

        coverage[area.id()] = coveragePercent;
    }

    return coverage;
}

int AnalyticsController::getExpiringCertificationsCount(int daysThreshold)
{
    lastError_.clear();

    CertificationController certCtrl;
    auto expiringCerts = certCtrl.getExpiringCertifications(daysThreshold);

    return expiringCerts.size();
}
