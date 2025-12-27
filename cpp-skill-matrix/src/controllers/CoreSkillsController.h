#ifndef CORESKILLSCONTROLLER_H
#define CORESKILLSCONTROLLER_H

#include "../models/CoreSkillCategory.h"
#include "../models/CoreSkill.h"
#include "../models/CoreSkillAssessment.h"
#include <QList>
#include <QString>

/**
 * @brief Controller for Core Skills business logic
 *
 * Provides high-level operations for managing core skills assessments
 */
class CoreSkillsController
{
public:
    CoreSkillsController();
    ~CoreSkillsController();

    /**
     * @brief Get all categories and their skills
     * @return List of all categories with associated skills
     */
    QList<CoreSkillCategory> getAllCategories();

    /**
     * @brief Get all skills
     * @return List of all core skills
     */
    QList<CoreSkill> getAllSkills();

    /**
     * @brief Get all skills for a specific category
     * @param categoryId Category ID
     * @return List of skills in the category
     */
    QList<CoreSkill> getSkillsByCategory(const QString& categoryId);

    /**
     * @brief Get assessments for an engineer
     * @param engineerId Engineer ID
     * @return List of assessments for the engineer
     */
    QList<CoreSkillAssessment> getEngineerAssessments(const QString& engineerId);

    /**
     * @brief Update or create an assessment
     * @param engineerId Engineer ID
     * @param categoryId Category ID
     * @param skillId Skill ID
     * @param score Score (0-3)
     * @return true if successful
     */
    bool updateAssessment(const QString& engineerId, const QString& categoryId,
                         const QString& skillId, int score);

    /**
     * @brief Get completion percentage for an engineer
     * @param engineerId Engineer ID
     * @return Percentage of skills assessed (0-100)
     */
    double getCompletionPercentage(const QString& engineerId);

    /**
     * @brief Get average score for an engineer
     * @param engineerId Engineer ID
     * @return Average score across all assessed skills
     */
    double getAverageScore(const QString& engineerId);

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // CORESKILLSCONTROLLER_H
