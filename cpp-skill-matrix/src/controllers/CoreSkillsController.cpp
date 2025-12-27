#include "CoreSkillsController.h"
#include "../database/CoreSkillsRepository.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"

CoreSkillsController::CoreSkillsController() : lastError_("") {}
CoreSkillsController::~CoreSkillsController() {}

QList<CoreSkillCategory> CoreSkillsController::getAllCategories()
{
    lastError_.clear();
    CoreSkillsRepository repo;
    QList<CoreSkillCategory> categories = repo.findAllCategories();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("CoreSkillsController", "Failed to get categories: " + lastError_);
    }

    return categories;
}

QList<CoreSkill> CoreSkillsController::getAllSkills()
{
    lastError_.clear();
    CoreSkillsRepository repo;
    QList<CoreSkill> skills = repo.findAllSkills();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("CoreSkillsController", "Failed to get skills: " + lastError_);
    }

    return skills;
}

QList<CoreSkill> CoreSkillsController::getSkillsByCategory(const QString& categoryId)
{
    lastError_.clear();
    QList<CoreSkill> categorySkills;

    CoreSkillsRepository repo;
    QList<CoreSkill> allSkills = repo.findAllSkills();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("CoreSkillsController", "Failed to get skills: " + lastError_);
        return categorySkills;
    }

    // Filter skills by category
    for (const CoreSkill& skill : allSkills) {
        if (skill.categoryId() == categoryId) {
            categorySkills.append(skill);
        }
    }

    return categorySkills;
}

QList<CoreSkillAssessment> CoreSkillsController::getEngineerAssessments(const QString& engineerId)
{
    lastError_.clear();
    QList<CoreSkillAssessment> engineerAssessments;

    CoreSkillsRepository repo;
    QList<CoreSkillAssessment> allAssessments = repo.findAllAssessments();

    if (!repo.lastError().isEmpty()) {
        lastError_ = repo.lastError();
        Logger::instance().error("CoreSkillsController", "Failed to get assessments: " + lastError_);
        return engineerAssessments;
    }

    // Filter assessments by engineer
    for (const CoreSkillAssessment& assessment : allAssessments) {
        if (assessment.engineerId() == engineerId) {
            engineerAssessments.append(assessment);
        }
    }

    return engineerAssessments;
}

bool CoreSkillsController::updateAssessment(const QString& engineerId, const QString& categoryId,
                                            const QString& skillId, int score)
{
    lastError_.clear();

    // Validate score
    if (score < 0 || score > Constants::SCORE_MAX) {
        lastError_ = QString("Invalid score: %1. Must be between 0 and %2")
            .arg(score).arg(Constants::SCORE_MAX);
        Logger::instance().warning("CoreSkillsController", lastError_);
        return false;
    }

    // Validate engineer ID
    if (engineerId.isEmpty()) {
        lastError_ = "Engineer ID cannot be empty";
        Logger::instance().warning("CoreSkillsController", lastError_);
        return false;
    }

    // Validate skill ID
    if (skillId.isEmpty()) {
        lastError_ = "Skill ID cannot be empty";
        Logger::instance().warning("CoreSkillsController", lastError_);
        return false;
    }

    // Create assessment object
    CoreSkillAssessment assessment;
    assessment.setEngineerId(engineerId);
    assessment.setCategoryId(categoryId);
    assessment.setSkillId(skillId);
    assessment.setScore(score);

    // Save to repository
    CoreSkillsRepository repo;
    bool success = repo.saveOrUpdateAssessment(assessment);

    if (!success) {
        lastError_ = repo.lastError();
        Logger::instance().error("CoreSkillsController", "Failed to update assessment: " + lastError_);
    }

    return success;
}

double CoreSkillsController::getCompletionPercentage(const QString& engineerId)
{
    lastError_.clear();

    // Get total skills
    CoreSkillsRepository repo;
    QList<CoreSkill> allSkills = repo.findAllSkills();
    int totalSkills = allSkills.size();

    if (totalSkills == 0) {
        return 0.0;
    }

    // Get engineer's assessments
    QList<CoreSkillAssessment> assessments = getEngineerAssessments(engineerId);

    // Count assessed skills (score > 0)
    int assessedCount = 0;
    for (const CoreSkillAssessment& assessment : assessments) {
        if (assessment.score() > 0) {
            assessedCount++;
        }
    }

    return (double)assessedCount / totalSkills * 100.0;
}

double CoreSkillsController::getAverageScore(const QString& engineerId)
{
    lastError_.clear();

    // Get engineer's assessments
    QList<CoreSkillAssessment> assessments = getEngineerAssessments(engineerId);

    if (assessments.isEmpty()) {
        return 0.0;
    }

    // Calculate average of assessed skills (score > 0)
    int totalScore = 0;
    int assessedCount = 0;

    for (const CoreSkillAssessment& assessment : assessments) {
        if (assessment.score() > 0) {
            totalScore += assessment.score();
            assessedCount++;
        }
    }

    if (assessedCount == 0) {
        return 0.0;
    }

    return (double)totalScore / assessedCount;
}
