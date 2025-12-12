#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/CoreSkillsRepository.h"

// Forward declarations for QtCharts
QT_BEGIN_NAMESPACE
namespace QtCharts {
    class QChart;
    class QChartView;
    class QBarSeries;
    class QPieSeries;
}
QT_END_NAMESPACE

using namespace QtCharts;

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    ~DashboardWidget();

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadStatistics();
    void createSkillDistributionChart();
    void createRecentActivityFeed();
    void updateQuickStats();

private:
    // Stat labels
    QLabel* engineerCountLabel_;
    QLabel* productionAreaCountLabel_;
    QLabel* assessmentCountLabel_;
    QLabel* coreSkillCountLabel_;
    QLabel* lastUpdateLabel_;
    QLabel* lastLoginLabel_;

    // Charts
    QChartView* skillChartView_;
    QChart* skillChart_;

    // Recent activity
    QListWidget* recentActivityList_;

    // Buttons
    QPushButton* refreshButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
    CoreSkillsRepository coreSkillsRepo_;
};

#endif // DASHBOARDWIDGET_H
