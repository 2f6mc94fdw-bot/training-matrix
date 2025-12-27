#ifndef ASSESSMENTCONTROLLER_H
#define ASSESSMENTCONTROLLER_H

#include "../models/Assessment.h"
#include <QList>
#include <QString>

/**
 * @brief Controller for Machine Competency Assessment business logic
 *
 * Provides high-level operations for managing machine competency assessments
 */
class AssessmentController
{
public:
    AssessmentController();
    ~AssessmentController();

    /**
     * @brief Get all assessments
     * @return List of all assessments
     */
    QList<Assessment> getAllAssessments();

    /**
     * @brief Get assessments for an engineer
     * @param engineerId Engineer ID
     * @return List of assessments for the engineer
     */
    QList<Assessment> getAssessmentsByEngineer(const QString& engineerId);

    /**
     * @brief Get assessments for a production area
     * @param productionAreaId Production area ID
     * @return List of assessments in the production area
     */
    QList<Assessment> getAssessmentsByProductionArea(int productionAreaId);

    /**
     * @brief Get a specific assessment
     * @param engineerId Engineer ID
     * @param competencyId Competency ID
     * @return Assessment object, or invalid assessment if not found
     */
    Assessment getAssessment(const QString& engineerId, int competencyId);

    /**
     * @brief Update or create an assessment
     * @param engineerId Engineer ID
     * @param productionAreaId Production area ID
     * @param machineId Machine ID
     * @param competencyId Competency ID
     * @param score Score (0-3)
     * @return true if successful
     */
    bool updateAssessment(const QString& engineerId, int productionAreaId,
                         int machineId, int competencyId, int score);

    /**
     * @brief Delete an assessment
     * @param id Assessment ID
     * @return true if successful
     */
    bool deleteAssessment(int id);

    /**
     * @brief Get completion percentage for an engineer in a production area
     * @param engineerId Engineer ID
     * @param productionAreaId Production area ID
     * @return Percentage of competencies assessed (0-100)
     */
    double getCompletionPercentage(const QString& engineerId, int productionAreaId);

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // ASSESSMENTCONTROLLER_H
