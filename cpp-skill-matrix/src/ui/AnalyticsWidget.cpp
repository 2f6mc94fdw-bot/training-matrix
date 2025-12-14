#include "AnalyticsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QPolarChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAreaSeries>
#include <QMap>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
    , viewModeCombo_(nullptr)
    , productionAreaFilter_(nullptr)
    , engineerFilter_(nullptr)
    , skillChartTypeCombo_(nullptr)
    , breakdownChartTypeCombo_(nullptr)
    , totalAssessmentsLabel_(nullptr)
    , avgSkillLevelLabel_(nullptr)
    , engineersAssessedLabel_(nullptr)
    , coverageLabel_(nullptr)
    , skillDistributionChart_(nullptr)
    , breakdownChart_(nullptr)
{
    setupUI();
    loadAnalytics();
    Logger::instance().info("AnalyticsWidget", "Analytics widget initialized");
}

AnalyticsWidget::~AnalyticsWidget()
{
}

void AnalyticsWidget::setupUI()
{
    // Main scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Header with title and controls
    QVBoxLayout* headerLayout = new QVBoxLayout();

    // Title row
    QHBoxLayout* titleRow = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Analytics & Insights", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleRow->addWidget(titleLabel);
    titleRow->addStretch();

    // Refresh button
    QPushButton* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setMinimumWidth(100);
    connect(refreshButton, &QPushButton::clicked, this, &AnalyticsWidget::onRefreshClicked);
    titleRow->addWidget(refreshButton);

    headerLayout->addLayout(titleRow);

    // Filters row
    QHBoxLayout* filtersRow = new QHBoxLayout();
    filtersRow->setSpacing(12);

    // View mode
    QLabel* viewLabel = new QLabel("View:", this);
    viewModeCombo_ = new QComboBox(this);
    viewModeCombo_->addItem("Overall", OverallView);
    viewModeCombo_->addItem("By Production Area", ByProductionArea);
    viewModeCombo_->addItem("By Engineer", ByEngineer);
    viewModeCombo_->setMinimumWidth(180);
    connect(viewModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onViewModeChanged);
    filtersRow->addWidget(viewLabel);
    filtersRow->addWidget(viewModeCombo_);

    filtersRow->addSpacing(20);

    // Production area filter
    QLabel* areaLabel = new QLabel("Filter Area:", this);
    productionAreaFilter_ = new QComboBox(this);
    productionAreaFilter_->setMinimumWidth(200);
    connect(productionAreaFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onProductionAreaFilterChanged);
    filtersRow->addWidget(areaLabel);
    filtersRow->addWidget(productionAreaFilter_);

    filtersRow->addSpacing(20);

    // Engineer filter
    QLabel* engineerLabel = new QLabel("Filter Engineer:", this);
    engineerFilter_ = new QComboBox(this);
    engineerFilter_->setMinimumWidth(200);
    connect(engineerFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onEngineerFilterChanged);
    filtersRow->addWidget(engineerLabel);
    filtersRow->addWidget(engineerFilter_);

    filtersRow->addStretch();

    headerLayout->addLayout(filtersRow);
    mainLayout->addLayout(headerLayout);

    // Statistics Cards
    QGroupBox* statsGroup = new QGroupBox("Summary Statistics", this);
    QGridLayout* statsGrid = new QGridLayout(statsGroup);
    statsGrid->setSpacing(24);
    statsGrid->setContentsMargins(20, 20, 20, 20);

    // Create stat cards
    auto createStatCard = [](QGridLayout* grid, int row, int col, const QString& label, QLabel*& valueLabel) {
        QVBoxLayout* cardLayout = new QVBoxLayout();

        QLabel* titleLabel = new QLabel(label);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(11);
        titleFont.setWeight(QFont::Normal);
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("color: #94a3b8;");

        valueLabel = new QLabel("0");
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(36);
        valueFont.setWeight(QFont::Bold);
        valueLabel->setFont(valueFont);

        cardLayout->addWidget(valueLabel);
        cardLayout->addWidget(titleLabel);
        cardLayout->addStretch();

        QWidget* card = new QWidget();
        card->setLayout(cardLayout);
        grid->addWidget(card, row, col);
    };

    createStatCard(statsGrid, 0, 0, "Total Assessments", totalAssessmentsLabel_);
    createStatCard(statsGrid, 0, 1, "Avg Skill Level", avgSkillLevelLabel_);
    createStatCard(statsGrid, 0, 2, "Engineers Assessed", engineersAssessedLabel_);
    createStatCard(statsGrid, 0, 3, "Coverage", coverageLabel_);

    mainLayout->addWidget(statsGroup);

    // Charts Row
    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(24);

    // Skill Distribution Chart
    QGroupBox* skillChartGroup = new QGroupBox("Skill Level Distribution", this);
    QVBoxLayout* skillChartLayout = new QVBoxLayout(skillChartGroup);

    // Chart type selector for skill distribution
    QHBoxLayout* skillControlLayout = new QHBoxLayout();
    QLabel* skillChartLabel = new QLabel("Chart Type:", this);
    skillChartTypeCombo_ = new QComboBox(this);
    skillChartTypeCombo_->addItem("Bar Chart", BarChart);
    skillChartTypeCombo_->addItem("Pie Chart", PieChart);
    skillChartTypeCombo_->setMinimumWidth(120);
    connect(skillChartTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onSkillChartTypeChanged);
    skillControlLayout->addWidget(skillChartLabel);
    skillControlLayout->addWidget(skillChartTypeCombo_);
    skillControlLayout->addStretch();
    skillChartLayout->addLayout(skillControlLayout);

    skillDistributionChart_ = new QChartView(this);
    skillDistributionChart_->setRenderHint(QPainter::Antialiasing);
    skillDistributionChart_->setMinimumHeight(350);
    skillChartLayout->addWidget(skillDistributionChart_);
    chartsLayout->addWidget(skillChartGroup);

    // Breakdown Chart (Production Areas / Engineers)
    QGroupBox* breakdownChartGroup = new QGroupBox("Breakdown Analysis", this);
    QVBoxLayout* breakdownChartLayout = new QVBoxLayout(breakdownChartGroup);

    // Chart type selector for breakdown
    QHBoxLayout* breakdownControlLayout = new QHBoxLayout();
    QLabel* breakdownChartLabel = new QLabel("Chart Type:", this);
    breakdownChartTypeCombo_ = new QComboBox(this);
    breakdownChartTypeCombo_->addItem("Bar Chart", BarChart);
    breakdownChartTypeCombo_->addItem("Pie Chart", PieChart);
    breakdownChartTypeCombo_->addItem("Radar Chart", RadarChart);
    breakdownChartTypeCombo_->setMinimumWidth(120);
    connect(breakdownChartTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onBreakdownChartTypeChanged);
    breakdownControlLayout->addWidget(breakdownChartLabel);
    breakdownControlLayout->addWidget(breakdownChartTypeCombo_);
    breakdownControlLayout->addStretch();
    breakdownChartLayout->addLayout(breakdownControlLayout);

    breakdownChart_ = new QChartView(this);
    breakdownChart_->setRenderHint(QPainter::Antialiasing);
    breakdownChart_->setMinimumHeight(350);
    breakdownChartLayout->addWidget(breakdownChart_);
    chartsLayout->addWidget(breakdownChartGroup);

    mainLayout->addLayout(chartsLayout);
    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
    setLayout(outerLayout);
}

void AnalyticsWidget::loadAnalytics()
{
    // Load production areas
    productionAreaFilter_->clear();
    productionAreaFilter_->addItem("All Areas", 0);
    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        productionAreaFilter_->addItem(area.name(), area.id());
    }

    // Load engineers
    engineerFilter_->clear();
    engineerFilter_->addItem("All Engineers", "");
    QList<Engineer> engineers = engineerRepo_.findAll();
    for (const Engineer& engineer : engineers) {
        engineerFilter_->addItem(engineer.name(), engineer.id());
    }

    updateStatistics();
    updateSkillDistributionChart();
    updateBreakdownChart();
}

void AnalyticsWidget::updateStatistics()
{
    int filterAreaId = productionAreaFilter_->currentData().toInt();
    QString filterEngineerId = engineerFilter_->currentData().toString();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Filter by production area
    if (filterAreaId > 0) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.productionAreaId() == filterAreaId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    // Filter by engineer
    if (!filterEngineerId.isEmpty()) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.engineerId() == filterEngineerId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    // Calculate statistics
    int totalAssessments = assessments.size();
    totalAssessmentsLabel_->setText(QString::number(totalAssessments));

    if (totalAssessments > 0) {
        double totalScore = 0;
        for (const Assessment& a : assessments) {
            totalScore += a.score();
        }
        double avgSkill = totalScore / totalAssessments;
        avgSkillLevelLabel_->setText(QString::number(avgSkill, 'f', 2));

        QSet<QString> uniqueEngineers;
        for (const Assessment& a : assessments) {
            uniqueEngineers.insert(a.engineerId());
        }
        engineersAssessedLabel_->setText(QString::number(uniqueEngineers.size()));

        int nonZeroCount = 0;
        for (const Assessment& a : assessments) {
            if (a.score() > 0) {
                nonZeroCount++;
            }
        }
        double coverage = (double)nonZeroCount / totalAssessments * 100.0;
        coverageLabel_->setText(QString("%1%").arg(coverage, 0, 'f', 1));
    } else {
        avgSkillLevelLabel_->setText("0.00");
        engineersAssessedLabel_->setText("0");
        coverageLabel_->setText("0%");
    }

    Logger::instance().debug("AnalyticsWidget", QString("Updated statistics: %1 assessments").arg(totalAssessments));
}

void AnalyticsWidget::updateSkillDistributionChart()
{
    int filterAreaId = productionAreaFilter_->currentData().toInt();
    QString filterEngineerId = engineerFilter_->currentData().toString();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Apply filters
    if (filterAreaId > 0) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.productionAreaId() == filterAreaId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    if (!filterEngineerId.isEmpty()) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.engineerId() == filterEngineerId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    // Count by skill level
    QMap<QString, int> skillCounts;
    skillCounts["No Skill (0)"] = 0;
    skillCounts["Basic (1)"] = 0;
    skillCounts["Intermediate (2)"] = 0;
    skillCounts["Advanced (3)"] = 0;

    for (const Assessment& a : assessments) {
        switch (a.score()) {
            case 0: skillCounts["No Skill (0)"]++; break;
            case 1: skillCounts["Basic (1)"]++; break;
            case 2: skillCounts["Intermediate (2)"]++; break;
            case 3: skillCounts["Advanced (3)"]++; break;
        }
    }

    ChartType chartType = static_cast<ChartType>(skillChartTypeCombo_->currentData().toInt());

    if (chartType == PieChart) {
        createPieChart(skillDistributionChart_, skillCounts, "Skill Level Distribution");
    } else {
        createBarChart(skillDistributionChart_, skillCounts, "Skill Level Distribution");
    }
}

void AnalyticsWidget::updateBreakdownChart()
{
    ViewMode viewMode = static_cast<ViewMode>(viewModeCombo_->currentData().toInt());
    int filterAreaId = productionAreaFilter_->currentData().toInt();
    QString filterEngineerId = engineerFilter_->currentData().toString();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Apply filters
    if (filterAreaId > 0) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.productionAreaId() == filterAreaId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    if (!filterEngineerId.isEmpty()) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.engineerId() == filterEngineerId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    ChartType chartType = static_cast<ChartType>(breakdownChartTypeCombo_->currentData().toInt());
    QString title;

    if (viewMode == ByProductionArea) {
        // Count assessments by production area
        QMap<int, int> areaCounts;
        QMap<int, double> areaAvgScores;
        QMap<int, QString> areaNames;
        QMap<int, int> areaTotals;

        QList<ProductionArea> areas = productionRepo_.findAllAreas();
        for (const ProductionArea& area : areas) {
            areaCounts[area.id()] = 0;
            areaTotals[area.id()] = 0;
            areaNames[area.id()] = area.name();
        }

        for (const Assessment& a : assessments) {
            if (areaCounts.contains(a.productionAreaId())) {
                areaCounts[a.productionAreaId()]++;
                areaTotals[a.productionAreaId()] += a.score();
            }
        }

        if (chartType == RadarChart) {
            // Calculate average scores for radar chart
            QMap<QString, double> areaAvgMap;
            for (auto it = areaCounts.begin(); it != areaCounts.end(); ++it) {
                if (it.value() > 0) {
                    double avg = (double)areaTotals[it.key()] / it.value();
                    areaAvgMap[areaNames[it.key()]] = avg;
                }
            }
            title = "Average Skill by Production Area";
            createRadarChart(breakdownChart_, areaAvgMap, title);
        } else {
            // Use counts for bar/pie charts
            QMap<QString, int> areaCountMap;
            for (auto it = areaCounts.begin(); it != areaCounts.end(); ++it) {
                if (it.value() > 0) {
                    areaCountMap[areaNames[it.key()]] = it.value();
                }
            }
            title = "Assessments by Production Area";
            if (chartType == PieChart) {
                createPieChart(breakdownChart_, areaCountMap, title);
            } else {
                createBarChart(breakdownChart_, areaCountMap, title);
            }
        }
    } else if (viewMode == ByEngineer) {
        // Count assessments by engineer
        QMap<QString, int> engineerCounts;
        QMap<QString, int> engineerTotals;
        QMap<QString, QString> engineerNames;

        QList<Engineer> engineers = engineerRepo_.findAll();
        for (const Engineer& engineer : engineers) {
            engineerCounts[engineer.id()] = 0;
            engineerTotals[engineer.id()] = 0;
            engineerNames[engineer.id()] = engineer.name();
        }

        for (const Assessment& a : assessments) {
            if (engineerCounts.contains(a.engineerId())) {
                engineerCounts[a.engineerId()]++;
                engineerTotals[a.engineerId()] += a.score();
            }
        }

        if (chartType == RadarChart) {
            // Calculate average scores for radar chart
            QMap<QString, double> engineerAvgMap;
            for (auto it = engineerCounts.begin(); it != engineerCounts.end(); ++it) {
                if (it.value() > 0) {
                    double avg = (double)engineerTotals[it.key()] / it.value();
                    engineerAvgMap[engineerNames[it.key()]] = avg;
                }
            }
            title = "Average Skill by Engineer";
            createRadarChart(breakdownChart_, engineerAvgMap, title);
        } else {
            // Use counts for bar/pie charts
            QMap<QString, int> engineerCountMap;
            for (auto it = engineerCounts.begin(); it != engineerCounts.end(); ++it) {
                if (it.value() > 0) {
                    engineerCountMap[engineerNames[it.key()]] = it.value();
                }
            }
            title = "Assessments by Engineer";
            if (chartType == PieChart) {
                createPieChart(breakdownChart_, engineerCountMap, title);
            } else {
                createBarChart(breakdownChart_, engineerCountMap, title);
            }
        }
    } else {
        // Overall view - show production areas
        QMap<int, int> areaCounts;
        QMap<int, QString> areaNames;

        QList<ProductionArea> areas = productionRepo_.findAllAreas();
        for (const ProductionArea& area : areas) {
            areaCounts[area.id()] = 0;
            areaNames[area.id()] = area.name();
        }

        for (const Assessment& a : assessments) {
            if (areaCounts.contains(a.productionAreaId())) {
                areaCounts[a.productionAreaId()]++;
            }
        }

        QMap<QString, int> areaCountMap;
        for (auto it = areaCounts.begin(); it != areaCounts.end(); ++it) {
            if (it.value() > 0) {
                areaCountMap[areaNames[it.key()]] = it.value();
            }
        }

        title = "Assessments by Production Area";
        if (chartType == PieChart) {
            createPieChart(breakdownChart_, areaCountMap, title);
        } else if (chartType == RadarChart) {
            QMap<QString, double> doubleMap;
            for (auto it = areaCountMap.begin(); it != areaCountMap.end(); ++it) {
                doubleMap[it.key()] = it.value();
            }
            createRadarChart(breakdownChart_, doubleMap, title);
        } else {
            createBarChart(breakdownChart_, areaCountMap, title);
        }
    }
}

void AnalyticsWidget::createBarChart(QChartView* chartView, const QMap<QString, int>& data, const QString& title)
{
    QBarSet* set = new QBarSet("Count");
    QStringList categories;

    for (auto it = data.begin(); it != data.end(); ++it) {
        *set << it.value();
        categories << it.key();
    }

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chartView->setChart(chart);
}

void AnalyticsWidget::createPieChart(QChartView* chartView, const QMap<QString, int>& data, const QString& title)
{
    QPieSeries* series = new QPieSeries();

    for (auto it = data.begin(); it != data.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice* slice = series->append(it.key(), it.value());
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1: %2 (%3%)")
                .arg(it.key())
                .arg(it.value())
                .arg(100.0 * slice->percentage(), 0, 'f', 1));
        }
    }

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

    chartView->setChart(chart);
}

void AnalyticsWidget::createRadarChart(QChartView* chartView, const QMap<QString, double>& data, const QString& title)
{
    QPolarChart* chart = new QPolarChart();

    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setLabelFormat("%.1f");

    QLineSeries* series = new QLineSeries();

    int index = 0;
    for (auto it = data.begin(); it != data.end(); ++it) {
        series->append(index, it.value());
        angularAxis->append(it.key(), index);
        index++;
    }

    // Close the radar by connecting back to first point
    if (!data.isEmpty()) {
        series->append(index, data.begin().value());
    }

    chart->addSeries(series);
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    series->attachAxis(angularAxis);
    series->attachAxis(radialAxis);

    chart->setTitle(title);
    chart->legend()->setVisible(false);

    chartView->setChart(chart);
}

void AnalyticsWidget::onViewModeChanged(int index)
{
    updateBreakdownChart();
}

void AnalyticsWidget::onProductionAreaFilterChanged(int index)
{
    updateStatistics();
    updateSkillDistributionChart();
    updateBreakdownChart();
}

void AnalyticsWidget::onEngineerFilterChanged(int index)
{
    updateStatistics();
    updateSkillDistributionChart();
    updateBreakdownChart();
}

void AnalyticsWidget::onSkillChartTypeChanged(int index)
{
    updateSkillDistributionChart();
}

void AnalyticsWidget::onBreakdownChartTypeChanged(int index)
{
    updateBreakdownChart();
}

void AnalyticsWidget::onRefreshClicked()
{
    loadAnalytics();
}
