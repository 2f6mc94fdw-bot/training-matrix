#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <QString>

/**
 * @brief Excel file exporter (Phase 5 implementation)
 *
 * TODO: Implement using QXlsx library
 */
class ExcelExporter
{
public:
    ExcelExporter();
    ~ExcelExporter();

    /**
     * @brief Export all data to Excel file
     */
    bool exportAll(const QString& filePath);

    /**
     * @brief Export engineers to Excel file
     */
    bool exportEngineers(const QString& filePath);

    /**
     * @brief Export production areas to Excel file
     */
    bool exportProductionAreas(const QString& filePath);

    /**
     * @brief Export assessments to Excel file
     */
    bool exportAssessments(const QString& filePath);

    /**
     * @brief Export report to Excel file
     */
    bool exportReport(const QString& filePath, const QString& reportType);

    /**
     * @brief Get last error message
     */
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
    // TODO: Add QXlsx members
};

#endif // EXCELEXPORTER_H
