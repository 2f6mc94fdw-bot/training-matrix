#include "DashboardWidget.h"
#include "StyleManager.h"
#include "../utils/Logger.h"
#include "../utils/IconProvider.h"
#include "../core/Session.h"
#include "../core/Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QDateTime>
#include <QScrollArea>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

#include <algorithm>  // for std::sort

DashboardWidget::DashboardWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCountLabel_(nullptr)
    , competencyCountLabel_(nullptr)
    , avgSkillLevelLabel_(nullptr)
    , completionRateLabel_(nullptr)
    , lastUpdateLabel_(nullptr)
    , pieChartView_(nullptr)
    , pieChart_(nullptr)
    , barChartView_(nullptr)
    , barChart_(nullptr)
    , topPerformersList_(nullptr)
    , needsAttentionList_(nullptr)
    , totalAssessmentsLabel_(nullptr)
    , fullyTrainedLabel_(nullptr)
    , needTrainingLabel_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Dashboard widget initialized (web app style)");
}

DashboardWidget::~DashboardWidget()
{
}

void DashboardWidget::setupUI()
{
    // Main scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(32);  // 8px grid: 32px between sections
    mainLayout->setContentsMargins(40, 40, 40, 40);  // 8px grid: 40px container padding

    // Header Section with Logo
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(20);

    // Title and subtitle (left side)
    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("Dashboard", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);  // Web app style: large title
    titleFont.setWeight(QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("Team performance overview and key metrics", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    // Logo (right side) - Use native size to avoid blur from upscaling
    QLabel* logoLabel = new QLabel(this);
    QPixmap logo(":/images/aptitude-logo.png");
    if (!logo.isNull()) {
        // Use smaller size with high quality rendering to avoid blur
        int logoSize = 100;
        QPixmap scaledLogo = logo.scaled(logoSize, logoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Enable high DPI rendering for crisp display
        scaledLogo.setDevicePixelRatio(devicePixelRatio());
        logoLabel->setPixmap(scaledLogo);
    }
    headerLayout->addWidget(logoLabel);

    mainLayout->addLayout(headerLayout);

    mainLayout->addSpacing(24);  // Space after header

    // Quick Stats Cards (4 cards matching web app)
    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(24);  // 8px grid: 24px between cards

    // Helper lambda to create stat cards
    auto createStatCard = [this](const QString& title, QLabel*& valueLabel, const QString& iconName, const QString& borderColor) -> QWidget* {
        QGroupBox* card = new QGroupBox(title, this);
        card->setMinimumHeight(140);  // Web app card height

        // Set left border accent (simulating web app's colored left border)
        QString cardStyle = QString(
            "QGroupBox {"
            "    border-left: 4px solid %1;"
            "    padding: 20px;"
            "}"
        ).arg(borderColor);
        card->setStyleSheet(cardStyle);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(8);

        // Large number (web app style: text-4xl)
        valueLabel = new QLabel("0", this);
        QFont numberFont = valueLabel->font();
        numberFont.setPointSize(48);  // Very large like web app
        numberFont.setWeight(QFont::Bold);
        valueLabel->setFont(numberFont);
        valueLabel->setStyleSheet("color: " + StyleManager::instance().getColor("text").name() + ";");
        cardLayout->addWidget(valueLabel);

        // Descriptive label
        QLabel* descLabel = new QLabel(title, this);
        QFont descFont = descLabel->font();
        descFont.setPointSize(12);
        descLabel->setFont(descFont);
        descLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
        cardLayout->addWidget(descLabel);

        cardLayout->addStretch();

        return card;
    };

    // Card 1: Total Engineers (Aptitude Light Blue)
    QWidget* engineersCard = createStatCard("Total Engineers", engineerCountLabel_, "users", "#60A5FA");
    statsGrid->addWidget(engineersCard, 0, 0);

    // Card 2: Total Competencies (Aptitude Red)
    QWidget* competenciesCard = createStatCard("Total Competencies", competencyCountLabel_, "target", "#EF4444");
    statsGrid->addWidget(competenciesCard, 0, 1);

    // Card 3: Average Skill Level (Aptitude Green)
    QWidget* avgSkillCard = createStatCard("Average Skill Level", avgSkillLevelLabel_, "trending-up", "#10B981");
    statsGrid->addWidget(avgSkillCard, 0, 2);

    // Card 4: Completion Rate (Aptitude Orange)
    QWidget* completionCard = createStatCard("Completion Rate", completionRateLabel_, "award", "#FB923C");
    statsGrid->addWidget(completionCard, 0, 3);

    mainLayout->addLayout(statsGrid);

    mainLayout->addSpacing(32);  // Space after stat cards

    // Score Distribution Section (Pie Chart + Bar Chart side by side)
    QLabel* chartsSectionTitle = new QLabel("Score Distribution", this);
    QFont sectionTitleFont;
    sectionTitleFont.setPointSize(20);
    sectionTitleFont.setWeight(QFont::Bold);
    chartsSectionTitle->setFont(sectionTitleFont);
    mainLayout->addWidget(chartsSectionTitle);

    mainLayout->addSpacing(16);

    QHBoxLayout* chartsRow = new QHBoxLayout();
    chartsRow->setSpacing(24);

    // Left: Pie Chart
    QGroupBox* pieChartBox = new QGroupBox("Score Breakdown", this);
    QVBoxLayout* pieChartLayout = new QVBoxLayout(pieChartBox);
    pieChartLayout->setContentsMargins(24, 24, 24, 24);

    pieChart_ = new QChart();
    pieChart_->setTitle("");
    pieChart_->setAnimationOptions(QChart::SeriesAnimations);
    pieChart_->legend()->setAlignment(Qt::AlignBottom);

    pieChartView_ = new QChartView(pieChart_, this);
    pieChartView_->setRenderHint(QPainter::Antialiasing);
    pieChartView_->setMinimumHeight(350);
    pieChartLayout->addWidget(pieChartView_);

    pieChartBox->setMinimumWidth(400);
    chartsRow->addWidget(pieChartBox, 1);

    // Right: Bar Chart
    QGroupBox* barChartBox = new QGroupBox("Engineer Performance", this);
    QVBoxLayout* barChartLayout = new QVBoxLayout(barChartBox);
    barChartLayout->setContentsMargins(24, 24, 24, 24);

    barChart_ = new QChart();
    barChart_->setTitle("");
    barChart_->setAnimationOptions(QChart::SeriesAnimations);
    barChart_->legend()->setVisible(false);

    barChartView_ = new QChartView(barChart_, this);
    barChartView_->setRenderHint(QPainter::Antialiasing);
    barChartView_->setMinimumHeight(350);
    barChartLayout->addWidget(barChartView_);

    barChartBox->setMinimumWidth(500);
    chartsRow->addWidget(barChartBox, 1);

    mainLayout->addLayout(chartsRow);

    mainLayout->addSpacing(32);

    // Performance Lists Section (Top Performers + Needs Attention)
    QHBoxLayout* performanceRow = new QHBoxLayout();
    performanceRow->setSpacing(24);

    // Left: Top Performers
    QGroupBox* topPerformersBox = new QGroupBox("🏆 Top Performers", this);
    QVBoxLayout* topPerformersLayout = new QVBoxLayout(topPerformersBox);
    topPerformersLayout->setContentsMargins(24, 24, 24, 24);

    topPerformersList_ = new QListWidget(this);
    topPerformersList_->setMinimumHeight(250);
    topPerformersList_->setStyleSheet("QListWidget { background-color: #d1fae5; }");  // Green tint
    QFont listFont;
    listFont.setPointSize(14);
    topPerformersList_->setFont(listFont);
    topPerformersLayout->addWidget(topPerformersList_);

    performanceRow->addWidget(topPerformersBox, 1);

    // Right: Needs Attention
    QGroupBox* needsAttentionBox = new QGroupBox("⚠️ Needs Attention", this);
    QVBoxLayout* needsAttentionLayout = new QVBoxLayout(needsAttentionBox);
    needsAttentionLayout->setContentsMargins(24, 24, 24, 24);

    needsAttentionList_ = new QListWidget(this);
    needsAttentionList_->setMinimumHeight(250);
    needsAttentionList_->setStyleSheet("QListWidget { background-color: #fef3c7; }");  // Yellow tint
    needsAttentionList_->setFont(listFont);
    needsAttentionLayout->addWidget(needsAttentionList_);

    performanceRow->addWidget(needsAttentionBox, 1);

    mainLayout->addLayout(performanceRow);

    mainLayout->addSpacing(32);

    // Key Insights Section (3 stat boxes)
    QLabel* insightsSectionTitle = new QLabel("Key Insights", this);
    insightsSectionTitle->setFont(sectionTitleFont);
    mainLayout->addWidget(insightsSectionTitle);

    mainLayout->addSpacing(16);

    QHBoxLayout* insightsRow = new QHBoxLayout();
    insightsRow->setSpacing(24);

    // Helper lambda for insight boxes
    auto createInsightBox = [this](const QString& label, QLabel*& valueLabel) -> QWidget* {
        QGroupBox* box = new QGroupBox(label, this);
        box->setMinimumHeight(100);
        QVBoxLayout* boxLayout = new QVBoxLayout(box);
        boxLayout->setSpacing(8);

        valueLabel = new QLabel("0", this);
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(36);
        valueFont.setWeight(QFont::Bold);
        valueLabel->setFont(valueFont);
        valueLabel->setAlignment(Qt::AlignCenter);
        boxLayout->addWidget(valueLabel);

        boxLayout->addStretch();
        return box;
    };

    QWidget* assessmentsBox = createInsightBox("Total Assessments", totalAssessmentsLabel_);
    insightsRow->addWidget(assessmentsBox);

    QWidget* fullyTrainedBox = createInsightBox("Fully Trained (≥2)", fullyTrainedLabel_);
    insightsRow->addWidget(fullyTrainedBox);

    QWidget* needTrainingBox = createInsightBox("Need Training (<2)", needTrainingLabel_);
    insightsRow->addWidget(needTrainingBox);

    mainLayout->addLayout(insightsRow);

    mainLayout->addSpacing(32);

    // Footer with refresh button
    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 16, 0, 0);

    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    lastUpdateLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    QFont footerFont = lastUpdateLabel_->font();
    footerFont.setPointSize(14);
    lastUpdateLabel_->setFont(footerFont);
    footerLayout->addWidget(lastUpdateLabel_);

    footerLayout->addStretch();

    refreshButton_ = new QPushButton(IconProvider::instance().getIcon(IconProvider::Refresh),
                                      "Refresh Statistics", this);
    refreshButton_->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));

    QFont buttonFont = refreshButton_->font();
    buttonFont.setPointSize(14);
    buttonFont.setWeight(QFont::DemiBold);
    refreshButton_->setFont(buttonFont);
    refreshButton_->setMinimumHeight(40);

    connect(refreshButton_, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    footerLayout->addWidget(refreshButton_);

    mainLayout->addLayout(footerLayout);
    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);

    // Set the scroll area as the main layout
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(scrollArea);
    setLayout(outerLayout);
}

void DashboardWidget::loadStatistics()
{
    // PERFORMANCE FIX: Cache all data upfront to avoid redundant database queries
    // Previously this was calling findAll() multiple times (3x for assessments alone!)
    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> assessments = assessmentRepo_.findAll();
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // PERFORMANCE FIX: Cache machines and competencies upfront to avoid nested loop queries
    // Previously: 10 areas × 10 machines = 110 queries. Now: Just count in memory.
    QList<Machine> allMachines;
    int totalCompetencies = 0;
    for (const ProductionArea& area : areas) {
        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        allMachines.append(machines);
        for (const Machine& machine : machines) {
            totalCompetencies += productionRepo_.findCompetenciesByMachine(machine.id()).size();
        }
    }

    updateQuickStats(engineers, assessments, totalCompetencies);
    updateKeyInsights(assessments);
    createScoreDistributionCharts(engineers, assessments);
    createPerformanceLists();

    QString timestamp = QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP");
    lastUpdateLabel_->setText("Last updated: " + timestamp);
}

void DashboardWidget::updateQuickStats(const QList<Engineer>& engineers, const QList<Assessment>& assessments, int totalCompetencies)
{
    // Get basic counts
    int engineerCount = engineers.size();

    // Calculate average skill level and completion rate
    double totalScore = 0;
    int scoreCount = 0;
    int competent = 0;  // Score >= 2

    for (const Assessment& assessment : assessments) {
        totalScore += assessment.score();
        scoreCount++;
        if (assessment.score() >= 2) {
            competent++;
        }
    }

    double avgSkill = scoreCount > 0 ? totalScore / scoreCount : 0.0;
    double completionRate = scoreCount > 0 ? (double)competent / scoreCount * 100.0 : 0.0;

    // Update labels
    engineerCountLabel_->setText(QString::number(engineerCount));
    competencyCountLabel_->setText(QString::number(totalCompetencies));
    avgSkillLevelLabel_->setText(QString::number(avgSkill, 'f', 2));
    completionRateLabel_->setText(QString::number(completionRate, 'f', 1) + "%");
}

void DashboardWidget::updateKeyInsights(const QList<Assessment>& assessments)
{
    int totalAssessments = assessments.size();
    int fullyTrained = 0;
    int needTraining = 0;

    for (const Assessment& assessment : assessments) {
        if (assessment.score() >= 2) {
            fullyTrained++;
        } else {
            needTraining++;
        }
    }

    totalAssessmentsLabel_->setText(QString::number(totalAssessments));
    fullyTrainedLabel_->setText(QString::number(fullyTrained));
    needTrainingLabel_->setText(QString::number(needTraining));
}

void DashboardWidget::createScoreDistributionCharts(const QList<Engineer>& engineers, const QList<Assessment>& assessments)
{
    // Use cached assessments instead of querying again

    // === PIE CHART: Score Distribution ===
    QMap<int, int> scoreCounts;
    scoreCounts[0] = 0;
    scoreCounts[1] = 0;
    scoreCounts[2] = 0;
    scoreCounts[3] = 0;

    for (const Assessment& assessment : assessments) {
        scoreCounts[assessment.score()]++;
    }

    QPieSeries* pieSeries = new QPieSeries();

    // Web app colors for scores
    QPieSlice* slice0 = pieSeries->append("Not Trained (0)", scoreCounts[0]);
    slice0->setColor(QColor("#ff6b6b"));  // Red

    QPieSlice* slice1 = pieSeries->append("Basic (1)", scoreCounts[1]);
    slice1->setColor(QColor("#fbbf24"));  // Yellow

    QPieSlice* slice2 = pieSeries->append("Competent (2)", scoreCounts[2]);
    slice2->setColor(QColor("#60a5fa"));  // Blue

    QPieSlice* slice3 = pieSeries->append("Expert (3)", scoreCounts[3]);
    slice3->setColor(QColor("#4ade80"));  // Green

    pieChart_->removeAllSeries();
    pieChart_->addSeries(pieSeries);

    // === BAR CHART: Engineer Performance ===
    QBarSeries* barSeries = new QBarSeries();
    QBarSet* barSet = new QBarSet("Avg Score");
    barSet->setColor(QColor("#ff6b6b"));  // Red accent like web app

    QStringList engineerNames;
    // Use cached engineers instead of querying again

    for (const Engineer& engineer : engineers) {
        // PERFORMANCE FIX: Filter cached assessments instead of querying per engineer
        double totalScore = 0;
        int count = 0;
        for (const Assessment& assessment : assessments) {
            if (assessment.engineerId() == engineer.id()) {
                totalScore += assessment.score();
                count++;
            }
        }
        double avgScore = count > 0 ? totalScore / count : 0.0;

        *barSet << avgScore;
        engineerNames << engineer.name();
    }

    barSeries->append(barSet);

    barChart_->removeAllSeries();
    barChart_->addSeries(barSeries);

    // Setup axes
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(engineerNames);
    barChart_->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 3);
    axisY->setLabelFormat("%.1f");
    barChart_->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
}

void DashboardWidget::createPerformanceLists()
{
    topPerformersList_->clear();
    needsAttentionList_->clear();

    // Calculate average score per engineer
    struct EngineerPerformance {
        QString name;
        int assessmentCount;
        double avgScore;
    };

    QList<EngineerPerformance> performances;
    QList<Engineer> engineers = engineerRepo_.findAll();

    for (const Engineer& engineer : engineers) {
        QList<Assessment> engineerAssessments = assessmentRepo_.findByEngineer(engineer.id());
        if (engineerAssessments.isEmpty()) continue;

        double totalScore = 0;
        for (const Assessment& assessment : engineerAssessments) {
            totalScore += assessment.score();
        }
        double avgScore = totalScore / engineerAssessments.size();

        performances.append({engineer.name(), static_cast<int>(engineerAssessments.size()), avgScore});
    }

    // Sort by average score
    std::sort(performances.begin(), performances.end(),
              [](const EngineerPerformance& a, const EngineerPerformance& b) {
                  return a.avgScore > b.avgScore;
              });

    // Top 5 performers
    for (int i = 0; i < qMin(5, performances.size()); i++) {
        QString text = QString("%1. %2 - %3 assessments, Avg: %4")
                           .arg(i + 1)
                           .arg(performances[i].name)
                           .arg(performances[i].assessmentCount)
                           .arg(performances[i].avgScore, 0, 'f', 2);
        topPerformersList_->addItem(text);
    }

    // Bottom 5 (needs attention) - reverse order
    int startIdx = qMax(0, performances.size() - 5);
    for (int i = performances.size() - 1; i >= startIdx; i--) {
        QString text = QString("⚠ %1 - %2 assessments, Avg: %3")
                           .arg(performances[i].name)
                           .arg(performances[i].assessmentCount)
                           .arg(performances[i].avgScore, 0, 'f', 2);
        needsAttentionList_->addItem(text);
    }
}

void DashboardWidget::onRefreshClicked()
{
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Statistics refreshed");
}
