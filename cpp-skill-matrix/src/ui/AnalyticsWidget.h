#ifndef ANALYTICSWIDGET_H
#define ANALYTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include "../database/AssessmentRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"

class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(QWidget* parent = nullptr);
    ~AnalyticsWidget();

    enum ViewMode {
        OverallView,
        ByProductionArea,
        ByEngineer
    };

    enum ChartType {
        BarChart,
        PieChart,
        RadarChart
    };

private:
    void setupUI();
    void loadAnalytics();
    void updateSkillDistributionChart();
    void updateBreakdownChart();
    void updateStatistics();
    void createBarChart(QChartView* chartView, const QMap<QString, int>& data, const QString& title);
    void createPieChart(QChartView* chartView, const QMap<QString, int>& data, const QString& title);
    void createRadarChart(QChartView* chartView, const QMap<QString, double>& data, const QString& title);

    // UI Components - Filters and Controls
    QComboBox* viewModeCombo_;
    QComboBox* productionAreaFilter_;
    QComboBox* engineerFilter_;
    QComboBox* skillChartTypeCombo_;
    QComboBox* breakdownChartTypeCombo_;

    // UI Components - Statistics
    QLabel* totalAssessmentsLabel_;
    QLabel* avgSkillLevelLabel_;
    QLabel* engineersAssessedLabel_;
    QLabel* coverageLabel_;

    // UI Components - Charts
    QChartView* skillDistributionChart_;
    QChartView* breakdownChart_;

    // Repositories
    AssessmentRepository assessmentRepo_;
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;

private slots:
    void onViewModeChanged(int index);
    void onProductionAreaFilterChanged(int index);
    void onEngineerFilterChanged(int index);
    void onSkillChartTypeChanged(int index);
    void onBreakdownChartTypeChanged(int index);
    void onRefreshClicked();
};

#endif // ANALYTICSWIDGET_H
