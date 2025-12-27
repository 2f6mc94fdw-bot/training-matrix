#ifndef REPORTCONTROLLER_H
#define REPORTCONTROLLER_H

#include <QString>
#include <QMap>

/**
 * @brief Controller for Report generation business logic
 */
class ReportController
{
public:
    ReportController();
    ~ReportController();

    /**
     * @brief Generate engineer skills summary report
     * @param engineerId Engineer ID
     * @return Map of report data (keys: total_skills, assessed_skills, avg_score, etc.)
     */
    QMap<QString, QString> generateEngineerReport(const QString& engineerId);

    /**
     * @brief Generate production area coverage report
     * @param productionAreaId Production area ID
     * @return Map of report data (keys: total_competencies, assessed_count, coverage_percentage, etc.)
     */
    QMap<QString, QString> generateProductionAreaReport(int productionAreaId);

    /**
     * @brief Generate shift summary report
     * @param shift Shift name
     * @return Map of report data (keys: engineer_count, avg_competency, etc.)
     */
    QMap<QString, QString> generateShiftReport(const QString& shift);

    /**
     * @brief Generate overall system statistics
     * @return Map of report data (keys: total_engineers, total_assessments, etc.)
     */
    QMap<QString, QString> generateSystemReport();

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // REPORTCONTROLLER_H
