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

QT_CHARTS_USE_NAMESPACE

class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(QWidget* parent = nullptr);
    ~AnalyticsWidget();

private:
    void setupUI();
    void loadAnalytics();
    void updateSkillDistributionChart();
    void updateProductionAreaChart();
    void updateStatistics();

    // UI Components
    QLabel* totalAssessmentsLabel_;
    QLabel* avgSkillLevelLabel_;
    QLabel* engineersAssessedLabel_;
    QLabel* coverageLabel_;
    QComboBox* productionAreaFilter_;
    QChartView* skillDistributionChart_;
    QChartView* productionAreaChart_;

    // Repositories
    AssessmentRepository assessmentRepo_;
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;

private slots:
    void onProductionAreaFilterChanged(int index);
    void onRefreshClicked();
};

#endif // ANALYTICSWIDGET_H
