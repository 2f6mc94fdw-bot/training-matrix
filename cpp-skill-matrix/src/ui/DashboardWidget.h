#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"

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

private:
    QLabel* engineerCountLabel_;
    QLabel* productionAreaCountLabel_;
    QLabel* assessmentCountLabel_;
    QLabel* lastUpdateLabel_;
    QPushButton* refreshButton_;

    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
};

#endif // DASHBOARDWIDGET_H
