#ifndef ANALYTICSCONTROLLER_H
#define ANALYTICSCONTROLLER_H

#include <QString>
#include <QMap>
#include <QList>

/**
 * @brief Controller for Analytics and Statistics business logic
 */
class AnalyticsController
{
public:
    AnalyticsController();
    ~AnalyticsController();

    /**
     * @brief Get skill distribution statistics
     * @return Map of skill levels to counts (e.g., "basic": 10, "intermediate": 15, etc.)
     */
    QMap<QString, int> getSkillDistribution();

    /**
     * @brief Get assessment completion rates by shift
     * @return Map of shift names to completion percentages
     */
    QMap<QString, double> getCompletionRatesByShift();

    /**
     * @brief Get top performing engineers by average score
     * @param limit Number of engineers to return
     * @return List of engineer IDs sorted by performance
     */
    QList<QString> getTopPerformers(int limit = 10);

    /**
     * @brief Get engineers needing improvement (lowest scores)
     * @param limit Number of engineers to return
     * @return List of engineer IDs sorted by lowest performance
     */
    QList<QString> getEngineersNeedingImprovement(int limit = 10);

    /**
     * @brief Get machine competency coverage statistics
     * @return Map of production area IDs to coverage percentages
     */
    QMap<int, double> getProductionAreaCoverage();

    /**
     * @brief Get certification expiry alerts
     * @param daysThreshold Number of days before expiry to alert
     * @return Count of certifications expiring within threshold
     */
    int getExpiringCertificationsCount(int daysThreshold = 30);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // ANALYTICSCONTROLLER_H
