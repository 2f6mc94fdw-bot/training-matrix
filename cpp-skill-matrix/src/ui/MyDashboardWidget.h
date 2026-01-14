#ifndef MYDASHBOARDWIDGET_H
#define MYDASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/CoreSkillsRepository.h"
#include "../models/Engineer.h"
#include "../models/Assessment.h"

class MyDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyDashboardWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyDashboardWidget();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadDashboardData();
    void updatePersonalStats();
    void updateAreasOfWeakness();
    void createProductionAreaRadarChart();
    void createCoreSkillsRadarChart();
    QChart* createRadarChart(const QMap<QString, double>& data, const QString& title, const QColor& color);

private:
    QString engineerId_;
    bool isFirstShow_;

    // Personal stats labels
    QLabel* totalAssessmentsLabel_;
    QLabel* avgSkillLevelLabel_;
    QLabel* coreSkillsAvgLabel_;
    QLabel* competenciesAssessedLabel_;

    // Areas of weakness
    QListWidget* weaknessListWidget_;

    // Radar charts
    QChartView* productionAreaChartView_;
    QChartView* coreSkillsChartView_;

    // Buttons
    QPushButton* refreshButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
    CoreSkillsRepository coreSkillsRepo_;

    // Cached data
    Engineer currentEngineer_;
    QList<Assessment> assessments_;
    QList<CoreSkillAssessment> coreSkillAssessments_;
};

#endif // MYDASHBOARDWIDGET_H
