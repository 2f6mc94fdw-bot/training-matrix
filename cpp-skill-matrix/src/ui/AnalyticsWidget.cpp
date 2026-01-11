#include "AnalyticsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QShowEvent>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QCategoryAxis>
#include <QLinearGradient>
#include <QDateTime>
#include <algorithm>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
    , contentStack_(nullptr)
    , trendsButton_(nullptr)
    , shiftsButton_(nullptr)
    , insightsButton_(nullptr)
    , engineerRadarButton_(nullptr)
    , shiftOverviewButton_(nullptr)
    , currentCompletionLabel_(nullptr)
    , predictedCompletionLabel_(nullptr)
    , changeLabel_(nullptr)
    , trendChartView_(nullptr)
    , shiftCardsContainer_(nullptr)
    , shiftChartView_(nullptr)
    , insightsList_(nullptr)
    , engineerSelector_(nullptr)
    , engineerProductionRadarView_(nullptr)
    , engineerCoreSkillsRadarView_(nullptr)
    , shiftFilterCombo_(nullptr)
    , shiftDataTypeCombo_(nullptr)
    , shiftRadarContainer_(nullptr)
    , isFirstShow_(true)
    , engineerRadarChartHeight_(400)
    , shiftOverviewChartHeight_(500)
{
    setupUI();

    // Don't load analytics here - wait for showEvent() (lazy loading)
    Logger::instance().info("AnalyticsWidget", "Analytics widget initialized");
}

AnalyticsWidget::~AnalyticsWidget()
{
}

void AnalyticsWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // Lazy loading: only load data on first show
    if (isFirstShow_) {
        isFirstShow_ = false;
        loadAnalytics();
        Logger::instance().info("AnalyticsWidget", "Loaded analytics data on first show");
    }
}

void AnalyticsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QVBoxLayout* titleLayout = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("Advanced Analytics", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QLabel* subtitleLabel = new QLabel("Predictive insights, trends, and automated reporting", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #64748b;");

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    titleLayout->setSpacing(4);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    // Refresh button
    QPushButton* refreshButton = new QPushButton("Refresh", this);
    QFont buttonFont = refreshButton->font();
    buttonFont.setPointSize(14);
    buttonFont.setWeight(QFont::DemiBold);
    refreshButton->setFont(buttonFont);
    refreshButton->setMinimumHeight(40);
    refreshButton->setMinimumWidth(120);
    refreshButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ff6b6b;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff5252;"
        "}"
    );
    connect(refreshButton, &QPushButton::clicked, this, &AnalyticsWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton);

    mainLayout->addLayout(headerLayout);
    mainLayout->addSpacing(8);

    // Horizontal tab navigation
    QWidget* tabBar = new QWidget(this);
    tabBar->setStyleSheet(
        "QWidget {"
        "    background-color: transparent;"
        "}"
    );

    QHBoxLayout* tabBarLayout = new QHBoxLayout(tabBar);
    tabBarLayout->setSpacing(8);
    tabBarLayout->setContentsMargins(0, 0, 0, 0);

    // Create horizontal tab button function
    auto createTabButton = [this](const QString& text) -> QPushButton* {
        QPushButton* btn = new QPushButton(text, this);
        QFont btnFont = btn->font();
        btnFont.setPointSize(13);
        btnFont.setWeight(QFont::DemiBold);
        btn->setFont(btnFont);
        btn->setMinimumHeight(44);
        btn->setMinimumWidth(140);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    border-radius: 8px;"
            "    background-color: #f1f5f9;"
            "    color: #64748b;"
            "    padding: 10px 20px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #e2e8f0;"
            "    color: #475569;"
            "}"
        );
        return btn;
    };

    trendsButton_ = createTabButton("Trends");
    shiftsButton_ = createTabButton("Shift Comparison");
    insightsButton_ = createTabButton("Insights");
    engineerRadarButton_ = createTabButton("Engineer Radar");
    shiftOverviewButton_ = createTabButton("Shift Overview");

    tabBarLayout->addWidget(trendsButton_);
    tabBarLayout->addWidget(shiftsButton_);
    tabBarLayout->addWidget(insightsButton_);
    tabBarLayout->addWidget(engineerRadarButton_);
    tabBarLayout->addWidget(shiftOverviewButton_);
    tabBarLayout->addStretch();

    mainLayout->addWidget(tabBar);
    mainLayout->addSpacing(16);

    // Stacked widget for tab content
    contentStack_ = new QStackedWidget(this);
    contentStack_->setStyleSheet(
        "QStackedWidget {"
        "    background-color: transparent;"
        "}"
    );

    // Create tabs
    QWidget* trendsWidget = new QWidget();
    QWidget* shiftsWidget = new QWidget();
    QWidget* insightsWidget = new QWidget();
    QWidget* engineerRadarWidget = new QWidget();
    QWidget* shiftOverviewWidget = new QWidget();

    setupTrendsTab(trendsWidget);
    setupShiftComparisonTab(shiftsWidget);
    setupAutomatedInsightsTab(insightsWidget);
    setupEngineerRadarTab(engineerRadarWidget);
    setupShiftOverviewTab(shiftOverviewWidget);

    contentStack_->addWidget(trendsWidget);         // Index 0
    contentStack_->addWidget(shiftsWidget);         // Index 1
    contentStack_->addWidget(insightsWidget);       // Index 2
    contentStack_->addWidget(engineerRadarWidget);  // Index 3
    contentStack_->addWidget(shiftOverviewWidget);  // Index 4

    connect(trendsButton_, &QPushButton::clicked, [this]() { onTabChanged(0); });
    connect(shiftsButton_, &QPushButton::clicked, [this]() { onTabChanged(1); });
    connect(insightsButton_, &QPushButton::clicked, [this]() { onTabChanged(2); });
    connect(engineerRadarButton_, &QPushButton::clicked, [this]() { onTabChanged(3); });
    connect(shiftOverviewButton_, &QPushButton::clicked, [this]() { onTabChanged(4); });

    mainLayout->addWidget(contentStack_, 1);

    setLayout(mainLayout);

    // Set initial tab
    onTabChanged(0);
}

void AnalyticsWidget::setupTrendsTab(QWidget* trendsWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(trendsWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    // Prediction cards
    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(16);

    auto createPredictionCard = [this](const QString& title, const QString& borderColor, QLabel*& valueLabel) -> QWidget* {
        QGroupBox* card = new QGroupBox(this);
        card->setStyleSheet(QString(
            "QGroupBox {"
            "    background-color: white;"
            "    border-left: 4px solid %1;"
            "    border-radius: 8px;"
            "    padding: 20px;"
            "}"
        ).arg(borderColor));
        card->setMinimumHeight(120);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(8);

        QLabel* titleLabel = new QLabel(title, this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(12);
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("color: #64748b;");
        cardLayout->addWidget(titleLabel);

        valueLabel = new QLabel("0.0%", this);
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(32);
        valueFont.setBold(true);
        valueLabel->setFont(valueFont);
        cardLayout->addWidget(valueLabel);

        cardLayout->addStretch();
        return card;
    };

    QWidget* currentCard = createPredictionCard("Current Completion", "#60a5fa", currentCompletionLabel_);
    QWidget* predictedCard = createPredictionCard("Predicted Next Period", "#4ade80", predictedCompletionLabel_);
    QWidget* changeCard = createPredictionCard("Expected Change", "#ff6b6b", changeLabel_);

    cardsLayout->addWidget(currentCard);
    cardsLayout->addWidget(predictedCard);
    cardsLayout->addWidget(changeCard);

    layout->addLayout(cardsLayout);

    // Trend chart
    QGroupBox* chartGroup = new QGroupBox("Historical Performance Trend", this);
    chartGroup->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    trendChartView_ = new QChartView(this);
    trendChartView_->setRenderHint(QPainter::Antialiasing);
    trendChartView_->setMinimumHeight(400);
    chartLayout->addWidget(trendChartView_);

    layout->addWidget(chartGroup);
    layout->addStretch();
}

void AnalyticsWidget::setupShiftComparisonTab(QWidget* shiftsWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(shiftsWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    // Shift cards container
    shiftCardsContainer_ = new QWidget(this);
    QHBoxLayout* cardsLayout = new QHBoxLayout(shiftCardsContainer_);
    cardsLayout->setSpacing(16);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(shiftCardsContainer_);

    // Shift comparison chart
    QGroupBox* chartGroup = new QGroupBox("Shift Performance Comparison", this);
    chartGroup->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    shiftChartView_ = new QChartView(this);
    shiftChartView_->setRenderHint(QPainter::Antialiasing);
    shiftChartView_->setMinimumHeight(400);
    chartLayout->addWidget(shiftChartView_);

    layout->addWidget(chartGroup);
    layout->addStretch();
}

void AnalyticsWidget::setupAutomatedInsightsTab(QWidget* insightsWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(insightsWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* insightsGroup = new QGroupBox("Automated Insights", this);
    insightsGroup->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* insightsLayout = new QVBoxLayout(insightsGroup);
    insightsList_ = new QListWidget(this);
    insightsList_->setSpacing(12);
    insightsList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item {"
        "    border-radius: 8px;"
        "    padding: 16px;"
        "    margin-bottom: 8px;"
        "}"
    );
    insightsLayout->addWidget(insightsList_);

    layout->addWidget(insightsGroup);
}

void AnalyticsWidget::loadAnalytics()
{
    Logger::instance().info("AnalyticsWidget", "Loading analytics data...");

    // Load all data once and cache it (optimized for performance)
    cachedEngineers_ = engineerRepo_.findAll();
    cachedAssessments_ = assessmentRepo_.findAll();
    cachedAreas_ = productionRepo_.findAllAreas();

    // Pre-calculate total competencies to avoid repeated queries
    cachedTotalCompetencies_ = 0;
    for (const ProductionArea& area : cachedAreas_) {
        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> competencies = productionRepo_.findCompetenciesByMachine(machine.id());
            cachedTotalCompetencies_ += competencies.size();
        }
    }

    Logger::instance().info("AnalyticsWidget", "Data loaded. Updating analytics views...");

    // Update all analytics views
    updateTrendsData();
    updateShiftComparisonData();
    updateAutomatedInsights();

    // Populate engineer selector dropdown
    if (engineerSelector_) {
        engineerSelector_->clear();
        for (const Engineer& engineer : cachedEngineers_) {
            engineerSelector_->addItem(engineer.name(), engineer.id());
        }
    }

    // Populate shift filter dropdown with unique shifts
    if (shiftFilterCombo_) {
        QString currentShift = shiftFilterCombo_->currentData().toString();
        shiftFilterCombo_->clear();
        shiftFilterCombo_->addItem("All Shifts", "ALL");

        QSet<QString> uniqueShifts;
        for (const Engineer& engineer : cachedEngineers_) {
            QString shift = engineer.shift();
            if (!shift.isEmpty()) {
                uniqueShifts.insert(shift);
            }
        }

        QList<QString> shiftList = uniqueShifts.values();
        std::sort(shiftList.begin(), shiftList.end());

        for (const QString& shift : shiftList) {
            shiftFilterCombo_->addItem(shift, shift);
        }

        // Restore previous selection if it exists
        if (!currentShift.isEmpty()) {
            int index = shiftFilterCombo_->findData(currentShift);
            if (index >= 0) {
                shiftFilterCombo_->setCurrentIndex(index);
            }
        }
    }
}

void AnalyticsWidget::updateTrendsData()
{
    PredictionData prediction = calculatePrediction();

    // Update labels
    currentCompletionLabel_->setText(QString::number(prediction.current, 'f', 1) + "%");
    predictedCompletionLabel_->setText(QString::number(prediction.predicted, 'f', 1) + "%");

    QString changeText = (prediction.change > 0 ? "+" : "") + QString::number(prediction.change, 'f', 1) + "%";
    changeLabel_->setText(changeText);

    if (prediction.trend == "up") {
        changeLabel_->setStyleSheet("color: #4ade80;");  // Green
    } else if (prediction.trend == "down") {
        changeLabel_->setStyleSheet("color: #fbbf24;");  // Yellow/warning
    } else {
        changeLabel_->setStyleSheet("color: #64748b;");  // Gray
    }

    // Create trend chart (simplified - using current data)
    QLineSeries* series = new QLineSeries();
    series->setName("Team Competency %");

    // For demo purposes, show current completion rate
    // In a real implementation, you'd track historical snapshots
    series->append(0, prediction.current);
    series->append(1, prediction.predicted);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("Time Period");
    axisX->setRange(0, 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Completion %");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Style the series with web app color
    QPen pen(QColor("#ff6b6b"));
    pen.setWidth(3);
    series->setPen(pen);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    trendChartView_->setChart(chart);
}

void AnalyticsWidget::updateShiftComparisonData()
{
    QList<ShiftStats> shifts = calculateShiftComparison();

    // Clear existing shift cards
    QLayoutItem* item;
    QHBoxLayout* cardsLayout = qobject_cast<QHBoxLayout*>(shiftCardsContainer_->layout());
    while ((item = cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // Create shift cards
    for (int i = 0; i < shifts.size(); i++) {
        const ShiftStats& shift = shifts[i];

        QString borderColor = "#60a5fa";  // Blue
        if (i == 0) borderColor = "#4ade80";  // Green for best
        else if (i == shifts.size() - 1) borderColor = "#fbbf24";  // Yellow for worst

        QGroupBox* card = new QGroupBox(this);
        card->setStyleSheet(QString(
            "QGroupBox {"
            "    background-color: white;"
            "    border-left: 4px solid %1;"
            "    border-radius: 8px;"
            "    padding: 20px;"
            "}"
        ).arg(borderColor));
        card->setMinimumHeight(120);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(8);

        QHBoxLayout* headerLayout = new QHBoxLayout();
        QLabel* shiftLabel = new QLabel(shift.shiftName, this);
        QFont shiftFont = shiftLabel->font();
        shiftFont.setPointSize(14);
        shiftFont.setBold(true);
        shiftLabel->setFont(shiftFont);
        headerLayout->addWidget(shiftLabel);

        if (i == 0) {
            QLabel* awardLabel = new QLabel("🏆", this);
            awardLabel->setFont(shiftFont);
            headerLayout->addWidget(awardLabel);
        }
        headerLayout->addStretch();

        cardLayout->addLayout(headerLayout);

        QLabel* percentLabel = new QLabel(QString::number(shift.averageCompletion, 'f', 1) + "%", this);
        QFont percentFont = percentLabel->font();
        percentFont.setPointSize(32);
        percentFont.setBold(true);
        percentLabel->setFont(percentFont);
        percentLabel->setStyleSheet("color: #ff6b6b;");
        cardLayout->addWidget(percentLabel);

        QLabel* engineersLabel = new QLabel(QString::number(shift.engineerCount) + " engineers", this);
        QFont engFont = engineersLabel->font();
        engFont.setPointSize(12);
        engineersLabel->setFont(engFont);
        engineersLabel->setStyleSheet("color: #64748b;");
        cardLayout->addWidget(engineersLabel);

        cardLayout->addStretch();
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();

    // Create shift comparison bar chart
    QBarSet* barSet = new QBarSet("Competency %");
    barSet->setColor(QColor("#ff6b6b"));

    QStringList shiftNames;
    for (const ShiftStats& shift : shifts) {
        *barSet << shift.averageCompletion;
        shiftNames << shift.shiftName;
    }

    QBarSeries* series = new QBarSeries();
    series->append(barSet);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(shiftNames);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Completion %");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    shiftChartView_->setChart(chart);
}

void AnalyticsWidget::updateAutomatedInsights()
{
    insightsList_->clear();

    QList<Insight> insights = generateAutomatedInsights();

    for (const Insight& insight : insights) {
        QListWidgetItem* item = new QListWidgetItem(insightsList_);

        QString backgroundColor, borderColor;
        if (insight.type == "positive") {
            backgroundColor = "#d1fae5";  // Light green
            borderColor = "#4ade80";      // Green
        } else if (insight.type == "warning") {
            backgroundColor = "#fef3c7";  // Light yellow
            borderColor = "#fbbf24";      // Yellow
        } else {  // alert
            backgroundColor = "#fee2e2";  // Light red
            borderColor = "#ff6b6b";      // Red
        }

        QString itemText = QString("%1  %2\n%3")
            .arg(insight.icon)
            .arg(insight.title)
            .arg(insight.message);

        item->setText(itemText);
        item->setBackground(QBrush(QColor(backgroundColor)));

        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);

        insightsList_->addItem(item);
    }

    if (insights.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(insightsList_);
        item->setText("👥  No Insights Available\nCollecting data to generate insights...\nAdd more assessments for automated analysis.");
        item->setBackground(QBrush(QColor("#f1f5f9")));
        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

AnalyticsWidget::PredictionData AnalyticsWidget::calculatePrediction()
{
    PredictionData result;
    result.current = 0.0;
    result.predicted = 0.0;
    result.change = 0.0;
    result.trend = "stable";

    // OPTIMIZATION: Use cached data
    if (cachedAssessments_.isEmpty()) {
        return result;
    }

    // Calculate current completion rate
    int totalScore = 0;
    int maxPossibleScore = cachedAssessments_.size() * 3;  // Max score is 3 per assessment

    for (const Assessment& assessment : cachedAssessments_) {
        totalScore += assessment.score();
    }

    result.current = maxPossibleScore > 0 ? (totalScore * 100.0) / maxPossibleScore : 0.0;

    // Simple prediction: assume 5% improvement
    // In a real implementation, you'd use historical trend data
    result.predicted = qMin(100.0, result.current + 5.0);
    result.change = result.predicted - result.current;

    if (result.change > 0.5) {
        result.trend = "up";
    } else if (result.change < -0.5) {
        result.trend = "down";
    } else {
        result.trend = "stable";
    }

    return result;
}

QList<AnalyticsWidget::ShiftStats> AnalyticsWidget::calculateShiftComparison()
{
    // OPTIMIZATION: Use cached data
    QMap<QString, ShiftStats> shiftsMap;

    // Group engineers by shift
    for (const Engineer& engineer : cachedEngineers_) {
        QString shift = engineer.shift();
        if (shift.isEmpty()) shift = "Unassigned";

        if (!shiftsMap.contains(shift)) {
            ShiftStats stats;
            stats.shiftName = shift;
            stats.averageCompletion = 0.0;
            stats.engineerCount = 0;
            stats.totalScore = 0;
            stats.maxScore = 0;
            shiftsMap[shift] = stats;
        }

        shiftsMap[shift].engineerCount++;

        // Calculate scores for this engineer using cached assessments
        for (const Assessment& assessment : cachedAssessments_) {
            if (assessment.engineerId() == engineer.id()) {
                shiftsMap[shift].totalScore += assessment.score();
                shiftsMap[shift].maxScore += 3;
            }
        }
    }

    // Calculate average completion for each shift
    QList<ShiftStats> result;
    for (auto it = shiftsMap.begin(); it != shiftsMap.end(); ++it) {
        ShiftStats stats = it.value();
        if (stats.maxScore > 0) {
            stats.averageCompletion = (stats.totalScore * 100.0) / stats.maxScore;
        }
        result.append(stats);
    }

    // Sort by average completion (descending)
    std::sort(result.begin(), result.end(),
              [](const ShiftStats& a, const ShiftStats& b) {
                  return a.averageCompletion > b.averageCompletion;
              });

    return result;
}

QList<AnalyticsWidget::Insight> AnalyticsWidget::generateAutomatedInsights()
{
    // OPTIMIZATION: Use cached data and reuse already calculated results
    QList<Insight> insights;

    PredictionData prediction = calculatePrediction();
    QList<ShiftStats> shifts = calculateShiftComparison();

    // Trend insight
    if (prediction.trend == "up") {
        Insight insight;
        insight.type = "positive";
        insight.icon = "📈";
        insight.title = "Positive Growth Trend";
        insight.message = QString("Team competency is projected to increase by %1% based on current progress.")
            .arg(QString::number(prediction.change, 'f', 1));
        insights.append(insight);
    } else if (prediction.trend == "down") {
        Insight insight;
        insight.type = "warning";
        insight.icon = "📉";
        insight.title = "Declining Trend Detected";
        insight.message = QString("Team competency is projected to decrease by %1%. Consider additional training initiatives.")
            .arg(QString::number(qAbs(prediction.change), 'f', 1));
        insights.append(insight);
    }

    // Shift performance gap
    if (shifts.size() > 1) {
        double gap = shifts[0].averageCompletion - shifts[shifts.size() - 1].averageCompletion;
        if (gap > 10.0) {
            Insight insight;
            insight.type = "alert";
            insight.icon = "⚠️";
            insight.title = "Shift Performance Gap";
            insight.message = QString("%1 outperforms %2 by %3%. Consider cross-shift training programs.")
                .arg(shifts[0].shiftName)
                .arg(shifts[shifts.size() - 1].shiftName)
                .arg(QString::number(gap, 'f', 1));
            insights.append(insight);
        }
    }

    // Low competency alert - use cached assessments
    int lowScores = 0;
    for (const Assessment& assessment : cachedAssessments_) {
        if (assessment.score() < 2) {
            lowScores++;
        }
    }

    if (!cachedAssessments_.isEmpty()) {
        double lowPercentage = (lowScores * 100.0) / cachedAssessments_.size();
        if (lowPercentage > 30.0) {
            Insight insight;
            insight.type = "warning";
            insight.icon = "🎯";
            insight.title = "Training Priority Alert";
            insight.message = QString("%1% of competencies are below proficient level. Prioritize structured training programs.")
                .arg(QString::number(lowPercentage, 'f', 0));
            insights.append(insight);
        }
    }

    // Top performer recognition - use cached data
    if (!cachedEngineers_.isEmpty()) {
        struct EngineerScore {
            QString name;
            double percentage;
        };
        QList<EngineerScore> engineerScores;

        for (const Engineer& engineer : cachedEngineers_) {
            int totalScore = 0;
            int maxScore = 0;

            for (const Assessment& assessment : cachedAssessments_) {
                if (assessment.engineerId() == engineer.id()) {
                    totalScore += assessment.score();
                    maxScore += 3;
                }
            }

            if (maxScore > 0) {
                EngineerScore score;
                score.name = engineer.name();
                score.percentage = (totalScore * 100.0) / maxScore;
                engineerScores.append(score);
            }
        }

        std::sort(engineerScores.begin(), engineerScores.end(),
                  [](const EngineerScore& a, const EngineerScore& b) {
                      return a.percentage > b.percentage;
                  });

        if (!engineerScores.isEmpty() && engineerScores[0].percentage >= 85.0) {
            Insight insight;
            insight.type = "positive";
            insight.icon = "🏆";
            insight.title = "Top Performer Recognition";
            insight.message = QString("%1 leads the team with %2% competency mastery.")
                .arg(engineerScores[0].name)
                .arg(QString::number(engineerScores[0].percentage, 'f', 0));
            insights.append(insight);
        }
    }

    return insights;
}

void AnalyticsWidget::onTabChanged(int tabIndex)
{
    contentStack_->setCurrentIndex(tabIndex);

    // Update button styles for horizontal tabs
    QString activeStyle =
        "QPushButton {"
        "    border: none;"
        "    border-radius: 8px;"
        "    background-color: #ff6b6b;"
        "    color: white;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff5252;"
        "}";

    QString inactiveStyle =
        "QPushButton {"
        "    border: none;"
        "    border-radius: 8px;"
        "    background-color: #f1f5f9;"
        "    color: #64748b;"
        "    padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "    color: #475569;"
        "}";

    trendsButton_->setStyleSheet(tabIndex == 0 ? activeStyle : inactiveStyle);
    shiftsButton_->setStyleSheet(tabIndex == 1 ? activeStyle : inactiveStyle);
    insightsButton_->setStyleSheet(tabIndex == 2 ? activeStyle : inactiveStyle);
    engineerRadarButton_->setStyleSheet(tabIndex == 3 ? activeStyle : inactiveStyle);
    shiftOverviewButton_->setStyleSheet(tabIndex == 4 ? activeStyle : inactiveStyle);

    // Update data for newly selected tab
    if (tabIndex == 3) {
        updateEngineerRadarData();
    } else if (tabIndex == 4) {
        updateShiftOverviewData();
    }
}

void AnalyticsWidget::onRefreshClicked()
{
    loadAnalytics();
    Logger::instance().info("AnalyticsWidget", "Refreshed analytics data");
}

// ============================================================================
// ENGINEER RADAR TAB
// ============================================================================

void AnalyticsWidget::setupEngineerRadarTab(QWidget* engineerRadarWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(engineerRadarWidget);
    layout->setSpacing(12);
    layout->setContentsMargins(0, 0, 0, 0);

    // Compact engineer selector - horizontal layout with label next to dropdown
    QWidget* selectorWidget = new QWidget(this);
    selectorWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "}"
    );

    QHBoxLayout* selectorLayout = new QHBoxLayout(selectorWidget);
    selectorLayout->setContentsMargins(16, 12, 16, 12);
    selectorLayout->setSpacing(12);

    QLabel* selectorLabel = new QLabel("Select Engineer:", this);
    QFont labelFont = selectorLabel->font();
    labelFont.setPointSize(13);
    labelFont.setWeight(QFont::DemiBold);
    selectorLabel->setFont(labelFont);
    selectorLabel->setStyleSheet("color: #475569; border: none;");

    engineerSelector_ = new QComboBox(this);
    engineerSelector_->setMinimumHeight(36);
    engineerSelector_->setMinimumWidth(250);
    QFont comboFont = engineerSelector_->font();
    comboFont.setPointSize(13);
    engineerSelector_->setFont(comboFont);
    engineerSelector_->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border: 2px solid #64748b;"
        "    width: 6px;"
        "    height: 6px;"
        "    border-top: none;"
        "    border-right: none;"
        "    transform: rotate(-45deg);"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #e0f2fe;"
        "    selection-color: #0c4a6e;"
        "    border: 1px solid #cbd5e1;"
        "}"
    );
    connect(engineerSelector_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onEngineerSelected);

    selectorLayout->addWidget(selectorLabel);
    selectorLayout->addWidget(engineerSelector_);
    selectorLayout->addStretch();

    // Zoom controls
    QLabel* zoomLabel = new QLabel("Chart Size:", this);
    zoomLabel->setFont(labelFont);
    zoomLabel->setStyleSheet("color: #475569; border: none;");
    selectorLayout->addWidget(zoomLabel);

    QPushButton* zoomOutBtn = new QPushButton("−", this);
    zoomOutBtn->setFixedSize(36, 36);
    zoomOutBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #64748b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "}"
    );
    connect(zoomOutBtn, &QPushButton::clicked, this, &AnalyticsWidget::onEngineerRadarZoomOut);
    selectorLayout->addWidget(zoomOutBtn);

    QPushButton* zoomInBtn = new QPushButton("+", this);
    zoomInBtn->setFixedSize(36, 36);
    zoomInBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #64748b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "}"
    );
    connect(zoomInBtn, &QPushButton::clicked, this, &AnalyticsWidget::onEngineerRadarZoomIn);
    selectorLayout->addWidget(zoomInBtn);

    layout->addWidget(selectorWidget);

    // Radar charts container - wrapped in scroll area for better display
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* chartsContainer = new QWidget();
    QHBoxLayout* chartsLayout = new QHBoxLayout(chartsContainer);
    chartsLayout->setSpacing(16);
    chartsLayout->setContentsMargins(0, 0, 0, 0);

    // Production Areas Radar
    engineerProductionRadarView_ = new QChartView(this);
    engineerProductionRadarView_->setRenderHint(QPainter::Antialiasing);
    engineerProductionRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    engineerProductionRadarView_->setMinimumWidth(500);
    engineerProductionRadarView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsLayout->addWidget(engineerProductionRadarView_, 1);

    // Core Skills Radar
    engineerCoreSkillsRadarView_ = new QChartView(this);
    engineerCoreSkillsRadarView_->setRenderHint(QPainter::Antialiasing);
    engineerCoreSkillsRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    engineerCoreSkillsRadarView_->setMinimumWidth(500);
    engineerCoreSkillsRadarView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsLayout->addWidget(engineerCoreSkillsRadarView_, 1);

    scrollArea->setWidget(chartsContainer);
    layout->addWidget(scrollArea, 1);
}

void AnalyticsWidget::onEngineerSelected(int index)
{
    updateEngineerRadarData();
}

void AnalyticsWidget::updateEngineerRadarData()
{
    if (!engineerSelector_ || engineerSelector_->currentIndex() < 0) {
        return;
    }

    QString engineerId = engineerSelector_->currentData().toString();
    if (engineerId.isEmpty()) {
        return;
    }

    // Calculate and display production areas radar
    QMap<QString, double> productionData = calculateEngineerProductionRadarData(engineerId);
    QPolarChart* productionChart = createRadarChart(
        productionData,
        "Production Areas Performance",
        QColor("#2196F3")  // Blue
    );
    engineerProductionRadarView_->setChart(productionChart);

    // Calculate and display core skills radar
    QMap<QString, double> coreSkillsData = calculateEngineerCoreSkillsRadarData(engineerId);
    QPolarChart* coreSkillsChart = createRadarChart(
        coreSkillsData,
        "Core Skills Performance",
        QColor("#FF9800")  // Orange
    );
    engineerCoreSkillsRadarView_->setChart(coreSkillsChart);

    Logger::instance().info("AnalyticsWidget", "Updated radar charts for engineer: " + engineerId);
}

// ============================================================================
// SHIFT OVERVIEW TAB
// ============================================================================

void AnalyticsWidget::setupShiftOverviewTab(QWidget* shiftOverviewWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(shiftOverviewWidget);
    layout->setSpacing(12);
    layout->setContentsMargins(0, 0, 0, 0);

    // Compact controls row - horizontal layout with labels next to dropdowns
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "}"
    );

    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(16, 12, 16, 12);
    controlsLayout->setSpacing(24);

    // Shift filter
    QLabel* filterLabel = new QLabel("Filter by Shift:", this);
    QFont labelFont = filterLabel->font();
    labelFont.setPointSize(13);
    labelFont.setWeight(QFont::DemiBold);
    filterLabel->setFont(labelFont);
    filterLabel->setStyleSheet("color: #475569; border: none;");

    shiftFilterCombo_ = new QComboBox(this);
    shiftFilterCombo_->setMinimumHeight(36);
    shiftFilterCombo_->setMinimumWidth(180);
    QFont comboFont = shiftFilterCombo_->font();
    comboFont.setPointSize(13);
    shiftFilterCombo_->setFont(comboFont);
    shiftFilterCombo_->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border: 2px solid #64748b;"
        "    width: 6px;"
        "    height: 6px;"
        "    border-top: none;"
        "    border-right: none;"
        "    transform: rotate(-45deg);"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #e0f2fe;"
        "    selection-color: #0c4a6e;"
        "    border: 1px solid #cbd5e1;"
        "}"
    );
    shiftFilterCombo_->addItem("All Shifts", "ALL");
    connect(shiftFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onShiftFilterChanged);

    controlsLayout->addWidget(filterLabel);
    controlsLayout->addWidget(shiftFilterCombo_);

    // Data type toggle
    QLabel* dataTypeLabel = new QLabel("Data Type:", this);
    dataTypeLabel->setFont(labelFont);
    dataTypeLabel->setStyleSheet("color: #475569; border: none;");

    shiftDataTypeCombo_ = new QComboBox(this);
    shiftDataTypeCombo_->setMinimumHeight(36);
    shiftDataTypeCombo_->setMinimumWidth(180);
    shiftDataTypeCombo_->setFont(comboFont);
    shiftDataTypeCombo_->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border: 2px solid #64748b;"
        "    width: 6px;"
        "    height: 6px;"
        "    border-top: none;"
        "    border-right: none;"
        "    transform: rotate(-45deg);"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #e0f2fe;"
        "    selection-color: #0c4a6e;"
        "    border: 1px solid #cbd5e1;"
        "}"
    );
    shiftDataTypeCombo_->addItem("Production Areas", "PRODUCTION");
    shiftDataTypeCombo_->addItem("Core Skills", "CORE_SKILLS");
    connect(shiftDataTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalyticsWidget::onShiftDataTypeChanged);

    controlsLayout->addWidget(dataTypeLabel);
    controlsLayout->addWidget(shiftDataTypeCombo_);
    controlsLayout->addStretch();

    // Zoom controls
    QLabel* zoomLabel = new QLabel("Chart Size:", this);
    zoomLabel->setFont(labelFont);
    zoomLabel->setStyleSheet("color: #475569; border: none;");
    controlsLayout->addWidget(zoomLabel);

    QPushButton* zoomOutBtn = new QPushButton("−", this);
    zoomOutBtn->setFixedSize(36, 36);
    zoomOutBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #64748b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "}"
    );
    connect(zoomOutBtn, &QPushButton::clicked, this, &AnalyticsWidget::onShiftOverviewZoomOut);
    controlsLayout->addWidget(zoomOutBtn);

    QPushButton* zoomInBtn = new QPushButton("+", this);
    zoomInBtn->setFixedSize(36, 36);
    zoomInBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #64748b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "}"
    );
    connect(zoomInBtn, &QPushButton::clicked, this, &AnalyticsWidget::onShiftOverviewZoomIn);
    controlsLayout->addWidget(zoomInBtn);

    layout->addWidget(controlsWidget);

    // Scroll area for shift radar charts
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    shiftRadarContainer_ = new QWidget();
    QVBoxLayout* containerLayout = new QVBoxLayout(shiftRadarContainer_);
    containerLayout->setSpacing(24);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(shiftRadarContainer_);
    layout->addWidget(scrollArea, 1);
}

void AnalyticsWidget::onShiftFilterChanged(int index)
{
    updateShiftOverviewData();
}

void AnalyticsWidget::onShiftDataTypeChanged(int index)
{
    updateShiftOverviewData();
}

void AnalyticsWidget::updateShiftOverviewData()
{
    if (!shiftRadarContainer_ || !shiftDataTypeCombo_) {
        return;
    }

    // Clear existing charts
    QLayout* layout = shiftRadarContainer_->layout();
    if (layout) {
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    }
    shiftRadarViews_.clear();

    // Get selected shift filter and data type
    QString selectedShift = shiftFilterCombo_->currentData().toString();
    QString dataType = shiftDataTypeCombo_->currentData().toString();
    bool isProductionData = (dataType == "PRODUCTION");

    // Get unique shifts from cached engineers
    QSet<QString> shifts;
    for (const Engineer& engineer : cachedEngineers_) {
        QString shift = engineer.shift();
        if (!shift.isEmpty()) {
            shifts.insert(shift);
        }
    }

    QList<QString> shiftList = shifts.values();
    std::sort(shiftList.begin(), shiftList.end());

    // If filter is not "ALL", only show selected shift
    if (selectedShift != "ALL" && !selectedShift.isEmpty()) {
        shiftList.clear();
        shiftList.append(selectedShift);
    }

    // Create single large radar chart for each shift showing individual engineers
    for (const QString& shift : shiftList) {
        // Get engineers in this shift
        QList<Engineer> shiftEngineers;
        for (const Engineer& engineer : cachedEngineers_) {
            if (engineer.shift() == shift) {
                shiftEngineers.append(engineer);
            }
        }

        if (shiftEngineers.isEmpty()) {
            continue;
        }

        // Shift header
        QLabel* shiftLabel = new QLabel(QString("Shift: %1 (%2 Engineers)").arg(shift).arg(shiftEngineers.size()), this);
        QFont headerFont = shiftLabel->font();
        headerFont.setPointSize(18);
        headerFont.setBold(true);
        shiftLabel->setFont(headerFont);
        shiftLabel->setStyleSheet("color: #1e293b; padding: 12px 0;");
        shiftRadarContainer_->layout()->addWidget(shiftLabel);

        // Create single large chart based on selected data type
        QMap<QString, QMap<QString, double>> engineerData;
        QString chartTitle;

        if (isProductionData) {
            for (const Engineer& engineer : shiftEngineers) {
                engineerData[engineer.name()] = calculateEngineerProductionRadarData(engineer.id());
            }
            chartTitle = QString("Production Areas - %1").arg(shift);
        } else {
            for (const Engineer& engineer : shiftEngineers) {
                engineerData[engineer.name()] = calculateEngineerCoreSkillsRadarData(engineer.id());
            }
            chartTitle = QString("Core Skills - %1").arg(shift);
        }

        QPolarChart* chart = createMultiEngineerRadarChart(engineerData, chartTitle, isProductionData);
        QChartView* chartView = new QChartView(chart, this);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(shiftOverviewChartHeight_);
        chartView->setMinimumWidth(800);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        shiftRadarContainer_->layout()->addWidget(chartView);
        shiftRadarViews_.append(chartView);

        // Divider
        QFrame* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("background-color: #e2e8f0; margin: 16px 0;");
        shiftRadarContainer_->layout()->addWidget(line);
    }

    // Add stretch at the end
    QVBoxLayout* containerLayout = qobject_cast<QVBoxLayout*>(shiftRadarContainer_->layout());
    if (containerLayout) {
        containerLayout->addStretch();
    }

    Logger::instance().info("AnalyticsWidget", QString("Updated shift overview for: %1, type: %2").arg(selectedShift).arg(dataType));
}

// ============================================================================
// RADAR CHART HELPER METHODS
// ============================================================================

QPolarChart* AnalyticsWidget::createRadarChart(const QMap<QString, double>& data,
                                                const QString& title,
                                                const QColor& color)
{
    QPolarChart* chart = new QPolarChart();
    chart->setTitle(title);
    chart->setAnimationOptions(QPolarChart::AllAnimations);

    // Prepare data
    QStringList labels;
    QList<double> values;
    int index = 0;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        labels.append(it.key());
        values.append(it.value());
    }

    if (labels.isEmpty()) {
        // No data - show empty chart with message
        chart->setTitle(title + " (No Data)");
        return chart;
    }

    // Create series
    QLineSeries* series = new QLineSeries();
    for (int i = 0; i < values.size(); i++) {
        series->append(i, values[i]);
    }
    // Close the polygon by connecting back to first point
    if (!values.isEmpty()) {
        series->append(values.size(), values.first());
    }

    // Create lower bound series at 0 for filling
    QLineSeries* lowerSeries = new QLineSeries();
    for (int i = 0; i <= values.size(); i++) {
        lowerSeries->append(i, 0);
    }

    // Create area series for filled effect
    QAreaSeries* areaSeries = new QAreaSeries(series, lowerSeries);
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(color.red(), color.green(), color.blue(), 50));
    areaSeries->setBrush(gradient);

    QPen pen(color);
    pen.setWidth(3);
    areaSeries->setPen(pen);

    chart->addSeries(areaSeries);

    // Angular axis (categories)
    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < labels.size(); i++) {
        angularAxis->append(abbreviateLabel(labels[i]), i);
    }
    angularAxis->setRange(0, labels.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    // Radial axis (values 0-3)
    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setTickCount(4);  // 0, 1, 2, 3
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    areaSeries->attachAxis(angularAxis);
    areaSeries->attachAxis(radialAxis);

    chart->legend()->setVisible(false);

    // Set consistent margins for all charts
    chart->setMargins(QMargins(20, 20, 20, 20));

    return chart;
}

QPolarChart* AnalyticsWidget::createMultiEngineerRadarChart(const QMap<QString, QMap<QString, double>>& engineerDataMap,
                                                             const QString& title,
                                                             bool isProductionData)
{
    QPolarChart* chart = new QPolarChart();
    chart->setTitle(title);
    chart->setAnimationOptions(QPolarChart::AllAnimations);

    if (engineerDataMap.isEmpty()) {
        chart->setTitle(title + " (No Data)");
        chart->legend()->setVisible(false);
        return chart;
    }

    // Collect all unique labels from all engineers
    QSet<QString> allLabelsSet;
    for (auto it = engineerDataMap.constBegin(); it != engineerDataMap.constEnd(); ++it) {
        const QMap<QString, double>& data = it.value();
        for (auto labelIt = data.constBegin(); labelIt != data.constEnd(); ++labelIt) {
            allLabelsSet.insert(labelIt.key());
        }
    }

    QStringList labels = allLabelsSet.values();
    std::sort(labels.begin(), labels.end());

    if (labels.isEmpty()) {
        chart->setTitle(title + " (No Data)");
        chart->legend()->setVisible(false);
        return chart;
    }

    // Predefined color palette for engineers
    QList<QColor> colorPalette = {
        QColor("#2196F3"), // Blue
        QColor("#FF9800"), // Orange
        QColor("#4CAF50"), // Green
        QColor("#F44336"), // Red
        QColor("#9C27B0"), // Purple
        QColor("#00BCD4"), // Cyan
        QColor("#FFEB3B"), // Yellow
        QColor("#795548"), // Brown
        QColor("#607D8B"), // Blue Grey
        QColor("#E91E63")  // Pink
    };

    // Create a series for each engineer
    int colorIndex = 0;
    for (auto it = engineerDataMap.constBegin(); it != engineerDataMap.constEnd(); ++it) {
        QString engineerName = it.key();
        const QMap<QString, double>& data = it.value();

        if (data.isEmpty()) {
            continue;
        }

        // Create data points for this engineer (in same label order)
        QLineSeries* series = new QLineSeries();
        series->setName(engineerName);

        for (int i = 0; i < labels.size(); i++) {
            double value = data.value(labels[i], 0.0);  // Default to 0 if no data for this label
            series->append(i, value);
        }
        // Close the polygon
        if (!labels.isEmpty()) {
            double firstValue = data.value(labels.first(), 0.0);
            series->append(labels.size(), firstValue);
        }

        // Create lower bound at 0
        QLineSeries* lowerSeries = new QLineSeries();
        for (int i = 0; i <= labels.size(); i++) {
            lowerSeries->append(i, 0);
        }

        // Create area series
        QAreaSeries* areaSeries = new QAreaSeries(series, lowerSeries);
        areaSeries->setName(engineerName);

        // Assign color from palette
        QColor engineerColor = colorPalette[colorIndex % colorPalette.size()];
        QPen pen(engineerColor);
        pen.setWidth(2);
        areaSeries->setPen(pen);

        // Semi-transparent fill for better overlap visibility
        QColor fillColor = engineerColor;
        fillColor.setAlpha(50);  // ~20% opacity for better overlap visibility
        areaSeries->setBrush(fillColor);

        chart->addSeries(areaSeries);
        colorIndex++;
    }

    // Angular axis (category labels)
    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < labels.size(); i++) {
        angularAxis->append(abbreviateLabel(labels[i]), i);
    }
    angularAxis->setRange(0, labels.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    // Radial axis (values 0-3)
    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setTickCount(4);  // 0, 1, 2, 3
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    // Attach axes to all series
    const QList<QAbstractSeries*> allSeries = chart->series();
    for (QAbstractSeries* series : allSeries) {
        series->attachAxis(angularAxis);
        series->attachAxis(radialAxis);
    }

    // Enable legend to show engineer names
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Set consistent margins for all charts
    chart->setMargins(QMargins(20, 20, 20, 20));

    return chart;
}

QMap<QString, double> AnalyticsWidget::calculateEngineerProductionRadarData(const QString& engineerId)
{
    QMap<QString, double> radarData;

    // Get all assessments for this engineer
    QList<Assessment> engineerAssessments;
    for (const Assessment& assessment : cachedAssessments_) {
        if (assessment.engineerId() == engineerId) {
            engineerAssessments.append(assessment);
        }
    }

    if (engineerAssessments.isEmpty()) {
        return radarData;
    }

    // Get all competencies and machines
    QList<Competency> allCompetencies;
    QList<Machine> allMachines;
    for (const ProductionArea& area : cachedAreas_) {
        QList<Machine> areaMachines = productionRepo_.findMachinesByArea(area.id());
        allMachines.append(areaMachines);
        for (const Machine& machine : areaMachines) {
            QList<Competency> machineCompetencies = productionRepo_.findCompetenciesByMachine(machine.id());
            allCompetencies.append(machineCompetencies);
        }
    }

    // Group by production area
    QMap<QString, double> areaWeightedSum;
    QMap<QString, double> areaTotalWeights;

    for (const Assessment& assessment : engineerAssessments) {
        // Find the competency
        for (const Competency& comp : allCompetencies) {
            if (comp.id() == assessment.competencyId()) {
                // Find the machine to get the area
                for (const Machine& machine : allMachines) {
                    if (machine.id() == comp.machineId()) {
                        // Find the area name
                        for (const ProductionArea& area : cachedAreas_) {
                            if (area.id() == machine.productionAreaId()) {
                                QString areaName = area.name();
                                // Combined weight: competency weight × machine production impact
                                double competencyWeight = comp.calculatedWeight();
                                double machineImpact = machine.importance(); // 0-3 scale
                                double combinedWeight = competencyWeight * (machineImpact + 1); // +1 to avoid zero weight
                                areaWeightedSum[areaName] += assessment.score() * combinedWeight;
                                areaTotalWeights[areaName] += combinedWeight;
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    // Calculate weighted averages
    for (auto it = areaWeightedSum.constBegin(); it != areaWeightedSum.constEnd(); ++it) {
        QString areaName = it.key();
        if (areaTotalWeights[areaName] > 0) {
            radarData[areaName] = it.value() / areaTotalWeights[areaName];
        }
    }

    return radarData;
}

QMap<QString, double> AnalyticsWidget::calculateEngineerCoreSkillsRadarData(const QString& engineerId)
{
    QMap<QString, double> radarData;

    // Get all core skill assessments for this engineer
    QList<CoreSkillAssessment> allAssessments = coreSkillsRepo_.findAllAssessments();
    QList<CoreSkillAssessment> engineerAssessments;
    for (const CoreSkillAssessment& assessment : allAssessments) {
        if (assessment.engineerId() == engineerId) {
            engineerAssessments.append(assessment);
        }
    }

    if (engineerAssessments.isEmpty()) {
        return radarData;
    }

    // Get all core skills and categories
    QList<CoreSkill> allSkills = coreSkillsRepo_.findAllSkills();
    QList<CoreSkillCategory> allCategories = coreSkillsRepo_.findAllCategories();

    // Group by category
    QMap<QString, double> categoryWeightedSum;
    QMap<QString, double> categoryTotalWeights;

    for (const CoreSkillAssessment& assessment : engineerAssessments) {
        // Find the skill
        for (const CoreSkill& skill : allSkills) {
            if (skill.id() == assessment.skillId()) {
                // Find the category
                for (const CoreSkillCategory& category : allCategories) {
                    if (category.id() == skill.categoryId()) {
                        QString categoryName = category.name();
                        double weight = skill.calculatedWeight();
                        categoryWeightedSum[categoryName] += assessment.score() * weight;
                        categoryTotalWeights[categoryName] += weight;
                        break;
                    }
                }
                break;
            }
        }
    }

    // Calculate weighted averages
    for (auto it = categoryWeightedSum.constBegin(); it != categoryWeightedSum.constEnd(); ++it) {
        QString categoryName = it.key();
        if (categoryTotalWeights[categoryName] > 0) {
            radarData[categoryName] = it.value() / categoryTotalWeights[categoryName];
        }
    }

    return radarData;
}

QMap<QString, double> AnalyticsWidget::calculateShiftProductionRadarData(const QString& shift)
{
    QMap<QString, double> aggregatedData;

    // Get all engineers in this shift
    QList<Engineer> shiftEngineers;
    for (const Engineer& engineer : cachedEngineers_) {
        if (engineer.shift() == shift) {
            shiftEngineers.append(engineer);
        }
    }

    if (shiftEngineers.isEmpty()) {
        return aggregatedData;
    }

    // Aggregate data from all engineers in the shift
    QMap<QString, double> totalScores;
    QMap<QString, int> areaCounts;

    for (const Engineer& engineer : shiftEngineers) {
        QMap<QString, double> engineerData = calculateEngineerProductionRadarData(engineer.id());
        for (auto it = engineerData.constBegin(); it != engineerData.constEnd(); ++it) {
            totalScores[it.key()] += it.value();
            areaCounts[it.key()]++;
        }
    }

    // Calculate averages
    for (auto it = totalScores.constBegin(); it != totalScores.constEnd(); ++it) {
        QString areaName = it.key();
        if (areaCounts[areaName] > 0) {
            aggregatedData[areaName] = it.value() / areaCounts[areaName];
        }
    }

    return aggregatedData;
}

QMap<QString, double> AnalyticsWidget::calculateShiftCoreSkillsRadarData(const QString& shift)
{
    QMap<QString, double> aggregatedData;

    // Get all engineers in this shift
    QList<Engineer> shiftEngineers;
    for (const Engineer& engineer : cachedEngineers_) {
        if (engineer.shift() == shift) {
            shiftEngineers.append(engineer);
        }
    }

    if (shiftEngineers.isEmpty()) {
        return aggregatedData;
    }

    // Aggregate data from all engineers in the shift
    QMap<QString, double> totalScores;
    QMap<QString, int> categoryCounts;

    for (const Engineer& engineer : shiftEngineers) {
        QMap<QString, double> engineerData = calculateEngineerCoreSkillsRadarData(engineer.id());
        for (auto it = engineerData.constBegin(); it != engineerData.constEnd(); ++it) {
            totalScores[it.key()] += it.value();
            categoryCounts[it.key()]++;
        }
    }

    // Calculate averages
    for (auto it = totalScores.constBegin(); it != totalScores.constEnd(); ++it) {
        QString categoryName = it.key();
        if (categoryCounts[categoryName] > 0) {
            aggregatedData[categoryName] = it.value() / categoryCounts[categoryName];
        }
    }

    return aggregatedData;
}

QString AnalyticsWidget::abbreviateLabel(const QString& label) const
{
    // Production area abbreviations to save space
    if (label.contains("Viaflo", Qt::CaseInsensitive)) {
        if (label.contains("Packing", Qt::CaseInsensitive)) {
            return "VFO P";
        }
        if (label.contains("Racking", Qt::CaseInsensitive)) {
            return "VFO R";
        }
        if (label.contains("Fill", Qt::CaseInsensitive)) {
            return "VFO F";
        }
        return "VFO";  // Generic Viaflo
    }

    if (label.contains("Line 6", Qt::CaseInsensitive) || label == "L6") {
        return "L6";
    }

    if (label.contains("Line 7", Qt::CaseInsensitive)) {
        if (label.contains("Packing", Qt::CaseInsensitive)) {
            return "L7 P";
        }
        return "L7";
    }

    if (label.contains("Line 1", Qt::CaseInsensitive) || label == "L1") {
        return "L1";
    }

    // If no abbreviation found, return original label
    return label;
}

void AnalyticsWidget::onEngineerRadarZoomIn()
{
    engineerRadarChartHeight_ += 100;
    if (engineerRadarChartHeight_ > 1400) {
        engineerRadarChartHeight_ = 1400;  // Max height
    }

    if (engineerProductionRadarView_) {
        engineerProductionRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    }
    if (engineerCoreSkillsRadarView_) {
        engineerCoreSkillsRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    }

    Logger::instance().info("AnalyticsWidget", QString("Engineer radar chart height: %1").arg(engineerRadarChartHeight_));
}

void AnalyticsWidget::onEngineerRadarZoomOut()
{
    engineerRadarChartHeight_ -= 100;
    if (engineerRadarChartHeight_ < 400) {
        engineerRadarChartHeight_ = 400;  // Min height
    }

    if (engineerProductionRadarView_) {
        engineerProductionRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    }
    if (engineerCoreSkillsRadarView_) {
        engineerCoreSkillsRadarView_->setMinimumHeight(engineerRadarChartHeight_);
    }

    Logger::instance().info("AnalyticsWidget", QString("Engineer radar chart height: %1").arg(engineerRadarChartHeight_));
}

void AnalyticsWidget::onShiftOverviewZoomIn()
{
    shiftOverviewChartHeight_ += 100;
    if (shiftOverviewChartHeight_ > 1600) {
        shiftOverviewChartHeight_ = 1600;  // Max height
    }

    // Update all existing shift charts
    for (QChartView* chartView : shiftRadarViews_) {
        if (chartView) {
            chartView->setMinimumHeight(shiftOverviewChartHeight_);
        }
    }

    Logger::instance().info("AnalyticsWidget", QString("Shift overview chart height: %1").arg(shiftOverviewChartHeight_));
}

void AnalyticsWidget::onShiftOverviewZoomOut()
{
    shiftOverviewChartHeight_ -= 100;
    if (shiftOverviewChartHeight_ < 500) {
        shiftOverviewChartHeight_ = 500;  // Min height
    }

    // Update all existing shift charts
    for (QChartView* chartView : shiftRadarViews_) {
        if (chartView) {
            chartView->setMinimumHeight(shiftOverviewChartHeight_);
        }
    }

    Logger::instance().info("AnalyticsWidget", QString("Shift overview chart height: %1").arg(shiftOverviewChartHeight_));
}
