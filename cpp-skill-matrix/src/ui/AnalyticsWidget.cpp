#include "AnalyticsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QLinearGradient>
#include <QDateTime>
#include <algorithm>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
    , contentStack_(nullptr)
    , trendsButton_(nullptr)
    , shiftsButton_(nullptr)
    , insightsButton_(nullptr)
    , currentCompletionLabel_(nullptr)
    , predictedCompletionLabel_(nullptr)
    , changeLabel_(nullptr)
    , trendChartView_(nullptr)
    , shiftCardsContainer_(nullptr)
    , shiftChartView_(nullptr)
    , insightsList_(nullptr)
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
    mainLayout->addSpacing(16);

    // Content area with sidebar + tabs
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(16);

    // Sidebar navigation
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "}"
    );

    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(4);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);

    QLabel* navLabel = new QLabel("ANALYTICS", this);
    QFont navFont = navLabel->font();
    navFont.setPointSize(11);
    navFont.setWeight(QFont::Bold);
    navLabel->setFont(navFont);
    navLabel->setStyleSheet("color: #94a3b8; padding: 8px;");
    sidebarLayout->addWidget(navLabel);

    // Tab buttons
    auto createTabButton = [this](const QString& icon, const QString& text) -> QPushButton* {
        QPushButton* btn = new QPushButton(icon + "  " + text, this);
        QFont btnFont = btn->font();
        btnFont.setPointSize(14);
        btn->setFont(btnFont);
        btn->setMinimumHeight(48);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "    text-align: left;"
            "    padding-left: 16px;"
            "    border: none;"
            "    border-radius: 8px;"
            "    background-color: transparent;"
            "    color: #475569;"
            "}"
            "QPushButton:hover {"
            "    background-color: #f1f5f9;"
            "}"
        );
        return btn;
    };

    trendsButton_ = createTabButton("📈", "Trends & Predictions");
    shiftsButton_ = createTabButton("🔄", "Shift Comparison");
    insightsButton_ = createTabButton("💡", "Automated Insights");

    sidebarLayout->addWidget(trendsButton_);
    sidebarLayout->addWidget(shiftsButton_);
    sidebarLayout->addWidget(insightsButton_);
    sidebarLayout->addStretch();

    contentLayout->addWidget(sidebar);

    // Stacked widget for tab content
    contentStack_ = new QStackedWidget(this);

    // Create tabs
    QWidget* trendsWidget = new QWidget();
    QWidget* shiftsWidget = new QWidget();
    QWidget* insightsWidget = new QWidget();

    setupTrendsTab(trendsWidget);
    setupShiftComparisonTab(shiftsWidget);
    setupAutomatedInsightsTab(insightsWidget);

    contentStack_->addWidget(trendsWidget);
    contentStack_->addWidget(shiftsWidget);
    contentStack_->addWidget(insightsWidget);

    connect(trendsButton_, &QPushButton::clicked, [this]() { onTabChanged(0); });
    connect(shiftsButton_, &QPushButton::clicked, [this]() { onTabChanged(1); });
    connect(insightsButton_, &QPushButton::clicked, [this]() { onTabChanged(2); });

    contentLayout->addWidget(contentStack_, 1);
    mainLayout->addLayout(contentLayout);

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
    updateTrendsData();
    updateShiftComparisonData();
    updateAutomatedInsights();
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

    QList<Assessment> assessments = assessmentRepo_.findAll();
    if (assessments.isEmpty()) {
        return result;
    }

    // Calculate current completion rate
    int totalScore = 0;
    int maxPossibleScore = assessments.size() * 3;  // Max score is 3 per assessment

    for (const Assessment& assessment : assessments) {
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
    QMap<QString, ShiftStats> shiftsMap;
    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Count total possible competencies
    int totalCompetenciesPerEngineer = 0;
    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> competencies = productionRepo_.findCompetenciesByMachine(machine.id());
            totalCompetenciesPerEngineer += competencies.size();
        }
    }

    // Group engineers by shift
    for (const Engineer& engineer : engineers) {
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

        // Calculate scores for this engineer
        for (const Assessment& assessment : assessments) {
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

    // Low competency alert
    QList<Assessment> assessments = assessmentRepo_.findAll();
    int lowScores = 0;
    for (const Assessment& assessment : assessments) {
        if (assessment.score() < 2) {
            lowScores++;
        }
    }

    if (!assessments.isEmpty()) {
        double lowPercentage = (lowScores * 100.0) / assessments.size();
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

    // Top performer recognition
    QList<Engineer> engineers = engineerRepo_.findAll();
    if (!engineers.isEmpty()) {
        struct EngineerScore {
            QString name;
            double percentage;
        };
        QList<EngineerScore> engineerScores;

        for (const Engineer& engineer : engineers) {
            int totalScore = 0;
            int maxScore = 0;

            for (const Assessment& assessment : assessments) {
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

    // Update button styles
    QString activeStyle =
        "QPushButton {"
        "    text-align: left;"
        "    padding-left: 16px;"
        "    border: none;"
        "    border-radius: 8px;"
        "    background-color: #ff6b6b;"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff5252;"
        "}";

    QString inactiveStyle =
        "QPushButton {"
        "    text-align: left;"
        "    padding-left: 16px;"
        "    border: none;"
        "    border-radius: 8px;"
        "    background-color: transparent;"
        "    color: #475569;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f1f5f9;"
        "}";

    trendsButton_->setStyleSheet(tabIndex == 0 ? activeStyle : inactiveStyle);
    shiftsButton_->setStyleSheet(tabIndex == 1 ? activeStyle : inactiveStyle);
    insightsButton_->setStyleSheet(tabIndex == 2 ? activeStyle : inactiveStyle);
}

void AnalyticsWidget::onRefreshClicked()
{
    loadAnalytics();
    Logger::instance().info("AnalyticsWidget", "Refreshed analytics data");
}
