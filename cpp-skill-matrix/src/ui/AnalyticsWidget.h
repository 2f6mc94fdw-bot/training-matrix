#ifndef ANALYTICSWIDGET_H
#define ANALYTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QComboBox>
#include <QHash>
#include "../database/AssessmentRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/CoreSkillsRepository.h"

class EngineerDevelopmentWidget;

class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(QWidget* parent = nullptr);
    ~AnalyticsWidget();

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void dataLoadingFinished();

public slots:
    void refresh() { loadAnalytics(); }  // Public refresh method

private slots:
    void onTabChanged(int tabIndex);
    void onRefreshClicked();
    void onEngineerSelected(int index);
    void onShiftFilterChanged(int index);
    void onShiftDataTypeChanged(int index);
    void onEngineerRadarZoomIn();
    void onEngineerRadarZoomOut();
    void onShiftOverviewZoomIn();
    void onShiftOverviewZoomOut();

private:
    void setupUI();
    void loadAnalytics();

    // Tab setup methods
    void setupTrendsTab(QWidget* trendsWidget);
    void setupShiftComparisonTab(QWidget* shiftsWidget);
    void setupAutomatedInsightsTab(QWidget* insightsWidget);
    void setupEngineerRadarTab(QWidget* engineerRadarWidget);
    void setupShiftOverviewTab(QWidget* shiftOverviewWidget);
    void setupCriticalSkillsTab(QWidget* criticalSkillsWidget);
    void setupMachineReadinessTab(QWidget* machineReadinessWidget);
    void ensureCoreSkillsCacheLoaded();

    // Data update methods
    void updateTrendsData();
    void updateShiftComparisonData();
    void updateAutomatedInsights();
    void updateEngineerRadarData();
    void updateShiftOverviewData();
    void updateCriticalSkillsData();
    void updateMachineReadinessData();

    // Helper methods
    struct PredictionData {
        double current;
        double predicted;
        double change;
        QString trend;  // "up", "down", "stable"
    };
    PredictionData calculatePrediction();

    struct ShiftStats {
        QString shiftName;
        double averageCompletion;
        int engineerCount;
        int totalScore;
        int maxScore;
    };
    QList<ShiftStats> calculateShiftComparison();

    struct Insight {
        QString type;  // "positive", "warning", "alert"
        QString icon;
        QString title;
        QString message;
    };
    QList<Insight> generateAutomatedInsights();
    QList<Insight> generateManagerInsights();

    // New data structures for advanced analytics
    struct CompetencyRiskPoint {
        QString name;
        int competencyId;
        double proficiency;      // 0-100% (team average)
        double importance;       // 0-5.0 from weight
        QString riskLevel;       // "critical", "maintain", "defer", "excess"
    };

    struct MachineReadiness {
        QString areaName;
        int areaId;
        QString machineName;
        int machineId;
        int proficientCount;     // engineers with score >= 2
        int expertCount;         // engineers with score == 3
        int totalEngineers;
        double coveragePercent;
        double averageScore;             // weighted mean of recorded assessments, 0-3
        double assessmentCoveragePercent;
        int competencyCount;
        bool isCritical;         // < 50% coverage
        int importance;
    };

    struct TrainingRecommendation {
        QString competencyName;
        int engineersNeed;
        double impactScore;      // 0-10
        QString reason;
        QString priority;        // "urgent", "high", "medium"
    };

    // Radar chart helper methods
    QPolarChart* createRadarChart(const QMap<QString, double>& data,
                                  const QString& title,
                                  const QColor& color);
    QPolarChart* createMultiEngineerRadarChart(const QMap<QString, QMap<QString, double>>& engineerDataMap,
                                               const QString& title,
                                               bool isProductionData);
    QMap<QString, double> calculateEngineerProductionRadarData(const QString& engineerId);
    QMap<QString, double> calculateEngineerCoreSkillsRadarData(const QString& engineerId);
    QMap<QString, double> calculateShiftProductionRadarData(const QString& shift);
    QMap<QString, double> calculateShiftCoreSkillsRadarData(const QString& shift);
    QString abbreviateLabel(const QString& label) const;

    // Advanced analytics calculation methods
    QList<CompetencyRiskPoint> calculateCompetencyRisks();
    QList<MachineReadiness> calculateMachineReadiness();
    QList<TrainingRecommendation> calculateTrainingROI();

private:
    // Navigation
    QStackedWidget* contentStack_;
    QPushButton* trendsButton_;
    QPushButton* shiftsButton_;
    QPushButton* insightsButton_;
    QPushButton* engineerRadarButton_;
    QPushButton* shiftOverviewButton_;
    QPushButton* criticalSkillsButton_;
    QPushButton* machineReadinessButton_;
    EngineerDevelopmentWidget* engineerDevelopmentWidget_;

    // Trends Tab Components
    QLabel* currentCompletionLabel_;
    QLabel* predictedCompletionLabel_;
    QLabel* changeLabel_;
    QChartView* trendChartView_;

    // Shift Comparison Tab Components
    QWidget* shiftCardsContainer_;
    QChartView* shiftChartView_;

    // Automated Insights Tab Components
    QListWidget* insightsList_;

    // Engineer Radar Tab Components
    QComboBox* engineerSelector_;
    QChartView* engineerProductionRadarView_;
    QChartView* engineerCoreSkillsRadarView_;

    // Critical Skills Tab Components
    QChartView* riskMatrixChartView_;
    QListWidget* trainingPriorityList_;

    // Machine Readiness Tab Components
    QComboBox* machineReadinessAreaFilter_;
    QListWidget* machineReadinessList_;
    QListWidget* vulnerabilityList_;

    // Shift Overview Tab Components
    QComboBox* shiftFilterCombo_;
    QComboBox* shiftDataTypeCombo_;
    QWidget* shiftRadarContainer_;
    QList<QChartView*> shiftRadarViews_;

    // Repositories
    AssessmentRepository assessmentRepo_;
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    CoreSkillsRepository coreSkillsRepo_;

    // Cached data for performance optimization
    QList<Engineer> cachedEngineers_;
    QList<Assessment> cachedAssessments_;
    QList<ProductionArea> cachedAreas_;
    QList<CoreSkill> cachedCoreSkills_;
    QList<CoreSkillCategory> cachedCoreSkillCategories_;
    QList<CoreSkillAssessment> cachedCoreSkillAssessments_;
    int cachedTotalCompetencies_;

    // Derived lookup caches for fast analytics calculations
    QHash<QString, QHash<int, int>> assessmentScoreByEngineerAndCompetency_;
    QHash<int, QList<Competency>> competenciesByMachine_;
    QHash<int, QList<Machine>> machinesByArea_;
    QHash<int, int> machineAreaById_;
    QHash<int, int> machineImportanceById_;
    QHash<int, double> competencyWeightById_;
    QHash<int, int> competencyMachineById_;
    QHash<int, QString> areaNameById_;
    QHash<QString, QList<CoreSkillAssessment>> coreSkillAssessmentsByEngineer_;
    QHash<QString, CoreSkill> coreSkillById_;
    QHash<QString, QString> coreSkillCategoryNameBySkillId_;
    QHash<QString, QString> coreSkillDisciplineBySkillId_;

    // Per-engineer memoization to speed repeated radar updates
    QHash<QString, QMap<QString, double>> engineerProductionRadarCache_;
    QHash<QString, QMap<QString, double>> engineerCoreSkillsRadarCache_;

    // Lazy loading state
    bool isFirstShow_;
    bool waitingForCacheWarmup_;
    bool insightsLoaded_;
    bool coreSkillsCacheLoaded_;

    // Chart sizing state
    int engineerRadarChartHeight_;
    int shiftOverviewChartHeight_;
};

#endif // ANALYTICSWIDGET_H
