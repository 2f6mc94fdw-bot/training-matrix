#ifndef ANALYTICSWIDGET_H
#define ANALYTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QtCharts/QChartView>
#include <QComboBox>
#include "../database/AssessmentRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/CoreSkillsRepository.h"

// Forward declaration
namespace QtCharts {
    class QPolarChart;
}

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

private:
    void setupUI();
    void loadAnalytics();

    // Tab setup methods
    void setupTrendsTab(QWidget* trendsWidget);
    void setupShiftComparisonTab(QWidget* shiftsWidget);
    void setupAutomatedInsightsTab(QWidget* insightsWidget);
    void setupEngineerRadarTab(QWidget* engineerRadarWidget);
    void setupShiftOverviewTab(QWidget* shiftOverviewWidget);

    // Data update methods
    void updateTrendsData();
    void updateShiftComparisonData();
    void updateAutomatedInsights();
    void updateEngineerRadarData();
    void updateShiftOverviewData();

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

    // Radar chart helper methods
    QtCharts::QPolarChart* createRadarChart(const QMap<QString, double>& data,
                                            const QString& title,
                                            const QColor& color);
    QMap<QString, double> calculateEngineerProductionRadarData(const QString& engineerId);
    QMap<QString, double> calculateEngineerCoreSkillsRadarData(const QString& engineerId);
    QMap<QString, double> calculateShiftProductionRadarData(const QString& shift);
    QMap<QString, double> calculateShiftCoreSkillsRadarData(const QString& shift);

private:
    // Navigation
    QStackedWidget* contentStack_;
    QPushButton* trendsButton_;
    QPushButton* shiftsButton_;
    QPushButton* insightsButton_;
    QPushButton* engineerRadarButton_;
    QPushButton* shiftOverviewButton_;

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

    // Shift Overview Tab Components
    QComboBox* shiftFilterCombo_;
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
    int cachedTotalCompetencies_;

    // Lazy loading state
    bool isFirstShow_;
};

#endif // ANALYTICSWIDGET_H
