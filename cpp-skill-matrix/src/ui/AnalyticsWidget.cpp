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
#include <QMap>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
    , totalAssessmentsLabel_(nullptr)
    , avgSkillLevelLabel_(nullptr)
    , engineersAssessedLabel_(nullptr)
    , coverageLabel_(nullptr)
    , productionAreaFilter_(nullptr)
    , skillDistributionChart_(nullptr)
    , productionAreaChart_(nullptr)
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

    // Title and filter
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Analytics & Insights", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // Production area filter
    QLabel* filterLabel = new QLabel("Filter by Area:", this);
    productionAreaFilter_ = new QComboBox(this);
    productionAreaFilter_->setMinimumWidth(200);
    connect(productionAreaFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onProductionAreaFilterChanged);
    headerLayout->addWidget(filterLabel);
    headerLayout->addWidget(productionAreaFilter_);

    // Refresh button
    QPushButton* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setMinimumWidth(100);
    connect(refreshButton, &QPushButton::clicked, this, &AnalyticsWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton);

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
    skillDistributionChart_ = new QChartView(this);
    skillDistributionChart_->setRenderHint(QPainter::Antialiasing);
    skillDistributionChart_->setMinimumHeight(300);
    skillChartLayout->addWidget(skillDistributionChart_);
    chartsLayout->addWidget(skillChartGroup);

    // Production Area Chart
    QGroupBox* areaChartGroup = new QGroupBox("Assessments by Production Area", this);
    QVBoxLayout* areaChartLayout = new QVBoxLayout(areaChartGroup);
    productionAreaChart_ = new QChartView(this);
    productionAreaChart_->setRenderHint(QPainter::Antialiasing);
    productionAreaChart_->setMinimumHeight(300);
    areaChartLayout->addWidget(productionAreaChart_);
    chartsLayout->addWidget(areaChartGroup);

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
    // Load production areas for filter
    productionAreaFilter_->clear();
    productionAreaFilter_->addItem("All Production Areas", 0);

    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        productionAreaFilter_->addItem(area.name(), area.id());
    }

    updateStatistics();
    updateSkillDistributionChart();
    updateProductionAreaChart();
}

void AnalyticsWidget::updateStatistics()
{
    int filterAreaId = productionAreaFilter_->currentData().toInt();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Filter by production area if selected
    if (filterAreaId > 0) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.productionAreaId() == filterAreaId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    // Calculate statistics
    int totalAssessments = assessments.size();
    totalAssessmentsLabel_->setText(QString::number(totalAssessments));

    if (totalAssessments > 0) {
        // Average skill level
        double totalScore = 0;
        for (const Assessment& a : assessments) {
            totalScore += a.score();
        }
        double avgSkill = totalScore / totalAssessments;
        avgSkillLevelLabel_->setText(QString::number(avgSkill, 'f', 2));

        // Unique engineers assessed
        QSet<QString> uniqueEngineers;
        for (const Assessment& a : assessments) {
            uniqueEngineers.insert(a.engineerId());
        }
        engineersAssessedLabel_->setText(QString::number(uniqueEngineers.size()));

        // Coverage (percentage of non-zero scores)
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
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Filter by production area if selected
    if (filterAreaId > 0) {
        QList<Assessment> filtered;
        for (const Assessment& a : assessments) {
            if (a.productionAreaId() == filterAreaId) {
                filtered.append(a);
            }
        }
        assessments = filtered;
    }

    // Count assessments by skill level
    QMap<int, int> skillCounts;
    skillCounts[0] = 0;
    skillCounts[1] = 0;
    skillCounts[2] = 0;
    skillCounts[3] = 0;

    for (const Assessment& a : assessments) {
        if (a.score() >= 0 && a.score() <= 3) {
            skillCounts[a.score()]++;
        }
    }

    // Create bar chart
    QBarSet* set = new QBarSet("Assessments");
    *set << skillCounts[0] << skillCounts[1] << skillCounts[2] << skillCounts[3];

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Distribution of Skill Levels");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "No Skill (0)" << "Basic (1)" << "Intermediate (2)" << "Advanced (3)";

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Number of Assessments");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    skillDistributionChart_->setChart(chart);
}

void AnalyticsWidget::updateProductionAreaChart()
{
    QList<Assessment> assessments = assessmentRepo_.findAll();
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Count assessments by production area
    QMap<int, int> areaCounts;
    QMap<int, QString> areaNames;

    for (const ProductionArea& area : areas) {
        areaCounts[area.id()] = 0;
        areaNames[area.id()] = area.name();
    }

    for (const Assessment& a : assessments) {
        if (areaCounts.contains(a.productionAreaId())) {
            areaCounts[a.productionAreaId()]++;
        }
    }

    // Create pie chart
    QPieSeries* series = new QPieSeries();

    for (auto it = areaCounts.begin(); it != areaCounts.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice* slice = series->append(areaNames[it.key()], it.value());
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1: %2 (%3%)")
                .arg(areaNames[it.key()])
                .arg(it.value())
                .arg(100.0 * slice->percentage(), 0, 'f', 1));
        }
    }

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Assessments by Production Area");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

    productionAreaChart_->setChart(chart);
}

void AnalyticsWidget::onProductionAreaFilterChanged(int index)
{
    updateStatistics();
    updateSkillDistributionChart();
}

void AnalyticsWidget::onRefreshClicked()
{
    loadAnalytics();
}
