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

    // Header Section
    QLabel* titleLabel = new QLabel("Dashboard", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);  // Web app style: large title
    titleFont.setWeight(QFont::Bold);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("Team performance overview and key metrics", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    mainLayout->addWidget(subtitleLabel);

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

    // Card 1: Total Engineers (gray border like web app)
    QWidget* engineersCard = createStatCard("Total Engineers", engineerCountLabel_, "users", "#64748b");
    statsGrid->addWidget(engineersCard, 0, 0);

    // Card 2: Total Competencies (red/accent border)
    QWidget* competenciesCard = createStatCard("Total Competencies", competencyCountLabel_, "target", "#ff6b6b");
    statsGrid->addWidget(competenciesCard, 0, 1);

    // Card 3: Average Skill Level (green/success border)
    QWidget* avgSkillCard = createStatCard("Average Skill Level", avgSkillLevelLabel_, "trending-up", "#10b981");
    statsGrid->addWidget(avgSkillCard, 0, 2);

    // Card 4: Completion Rate (yellow/warning border)
    QWidget* completionCard = createStatCard("Completion Rate", completionRateLabel_, "award", "#f59e0b");
    statsGrid->addWidget(completionCard, 0, 3);

    mainLayout->addLayout(statsGrid);

    QVBoxLayout* engineersLayout = new QVBoxLayout(engineersBox);
    engineersLayout->setSpacing(8);  // 8px grid: 8px between elements
    engineersLayout->setContentsMargins(24, 20, 24, 20);  // 8px grid: refined padding

    engineerCountLabel_ = new QLabel("0", this);
    QFont countFont = engineerCountLabel_->font();
    countFont.setPointSize(40);  // Typography scale: 40pt for large numbers (8px grid)
    countFont.setWeight(QFont::Bold);
    engineerCountLabel_->setFont(countFont);
    engineerCountLabel_->setAlignment(Qt::AlignCenter);
    engineerCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    engineersLayout->addWidget(engineerCountLabel_);

    QLabel* engineersSubtext = new QLabel("Active workforce", this);
    engineersSubtext->setAlignment(Qt::AlignCenter);
    QFont subtextFont = engineersSubtext->font();
    subtextFont.setPointSize(12);  // Typography scale: 12pt for small text
    subtextFont.setWeight(QFont::Normal);
    engineersSubtext->setFont(subtextFont);
    engineersSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    engineersLayout->addWidget(engineersSubtext);

    engineersBox->setMinimumHeight(120);  // 8px grid: 120px height
    engineersBox->setMinimumWidth(200);   // 8px grid: 200px width
    engineersBox->setMaximumWidth(280);   // 8px grid: 280px max width

    // Production Areas Card - Compact & Refined
    QGroupBox* areasBox = new QGroupBox("Production Areas", this);
    areasBox->setFont(cardTitleFont);

    QVBoxLayout* areasLayout = new QVBoxLayout(areasBox);
    areasLayout->setSpacing(8);  // 8px grid: 8px between elements
    areasLayout->setContentsMargins(24, 20, 24, 20);  // 8px grid: refined padding

    productionAreaCountLabel_ = new QLabel("0", this);
    productionAreaCountLabel_->setFont(countFont);
    productionAreaCountLabel_->setAlignment(Qt::AlignCenter);
    productionAreaCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    areasLayout->addWidget(productionAreaCountLabel_);

    QLabel* areasSubtext = new QLabel("Defined work areas", this);
    areasSubtext->setFont(subtextFont);
    areasSubtext->setAlignment(Qt::AlignCenter);
    areasSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    areasLayout->addWidget(areasSubtext);

    areasBox->setMinimumHeight(120);  // 8px grid: 120px height
    areasBox->setMinimumWidth(200);   // 8px grid: 200px width
    areasBox->setMaximumWidth(280);   // 8px grid: 280px max width

    statsLayout->addWidget(engineersBox);
    statsLayout->addWidget(areasBox);
    statsLayout->addStretch();

    mainLayout->addLayout(statsLayout);

    mainLayout->addSpacing(40);  // 8px grid: 40px before charts section

    // Charts and Activity Section
    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(24);  // 8px grid: 24px between chart panels

    // Skill Distribution Chart
    QGroupBox* chartBox = new QGroupBox("Skill Level Distribution", this);
    QFont chartTitleFont;
    chartTitleFont.setPointSize(16);  // Typography scale: 16pt for section titles
    chartTitleFont.setWeight(QFont::DemiBold);
    chartBox->setFont(chartTitleFont);

    QVBoxLayout* chartLayout = new QVBoxLayout(chartBox);
    chartLayout->setContentsMargins(24, 24, 24, 24);  // 8px grid: 24px padding

    skillChart_ = new QChart();
    skillChart_->setTitle("");
    skillChart_->setAnimationOptions(QChart::SeriesAnimations);
    skillChart_->legend()->setAlignment(Qt::AlignBottom);
    skillChart_->setMargins(QMargins(16, 16, 16, 16));  // 8px grid: 16px chart margins

    skillChartView_ = new QChartView(skillChart_, this);
    skillChartView_->setRenderHint(QPainter::Antialiasing);
    skillChartView_->setMinimumHeight(400);  // 8px grid: 400px height

    chartLayout->addWidget(skillChartView_);
    chartBox->setMinimumWidth(480);  // 8px grid: 480px width

    // Recent Activity Feed
    QGroupBox* activityBox = new QGroupBox("Recent Activity", this);
    activityBox->setFont(chartTitleFont);

    QVBoxLayout* activityLayout = new QVBoxLayout(activityBox);
    activityLayout->setContentsMargins(24, 24, 24, 24);  // 8px grid: 24px padding

    recentActivityList_ = new QListWidget(this);
    recentActivityList_->setMinimumHeight(400);  // 8px grid: 400px height
    recentActivityList_->setAlternatingRowColors(true);

    // Set font for list items
    QFont listFont;
    listFont.setPointSize(14);  // Typography scale: 14pt for list items
    recentActivityList_->setFont(listFont);

    activityLayout->addWidget(recentActivityList_);
    activityBox->setMinimumWidth(400);  // 8px grid: 400px width

    chartsLayout->addWidget(chartBox, 6);
    chartsLayout->addWidget(activityBox, 4);

    mainLayout->addLayout(chartsLayout);

    mainLayout->addSpacing(40);  // 8px grid: 40px before actions section

    // Quick Actions Guide
    QGroupBox* actionsBox = new QGroupBox("Quick Actions", this);
    actionsBox->setFont(chartTitleFont);

    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsBox);
    actionsLayout->setContentsMargins(32, 32, 32, 32);  // 8px grid: 32px padding

    QLabel* actionsLabel = new QLabel(
        "<ul style='line-height: 2.2; margin-left: -16px;'>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Engineers:</b> Manage engineer profiles and information</li>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Production Areas:</b> Define areas, machines, and competencies</li>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Assessments:</b> Perform and track competency evaluations</li>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Reports:</b> Generate detailed skill matrix reports</li>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Analytics:</b> View charts and insights</li>"
        "<li style='margin-bottom: 12px; font-size: 14pt;'><b>Import/Export:</b> Transfer data to/from external systems</li>"
        "</ul>", this);
    actionsLabel->setTextFormat(Qt::RichText);
    actionsLabel->setWordWrap(true);
    QFont actionsFont = actionsLabel->font();
    actionsFont.setPointSize(14);  // Typography scale: 14pt for body text
    actionsLabel->setFont(actionsFont);

    actionsLayout->addWidget(actionsLabel);
    mainLayout->addWidget(actionsBox);

    mainLayout->addSpacing(32);  // 8px grid: 32px before footer

    // Footer with last update and refresh button
    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 16, 0, 0);  // 8px grid: 16px top margin

    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    lastUpdateLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    QFont footerFont = lastUpdateLabel_->font();
    footerFont.setPointSize(14);  // Typography scale: 14pt for body text
    lastUpdateLabel_->setFont(footerFont);
    footerLayout->addWidget(lastUpdateLabel_);

    footerLayout->addStretch();

    refreshButton_ = new QPushButton(IconProvider::instance().getIcon(IconProvider::Refresh),
                                      "Refresh Statistics", this);
    refreshButton_->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));

    // Set button font
    QFont buttonFont = refreshButton_->font();
    buttonFont.setPointSize(14);  // Typography scale: 14pt for buttons
    buttonFont.setWeight(QFont::DemiBold);
    refreshButton_->setFont(buttonFont);
    refreshButton_->setMinimumHeight(40);  // 8px grid: 40px button height

    connect(refreshButton_, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    footerLayout->addWidget(refreshButton_);

    mainLayout->addLayout(footerLayout);
    mainLayout->addStretch();

    contentWidget->setLayout(mainLayout);

    // Wrap everything in a scroll area so spacing isn't compressed
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Set the scroll area as the main layout
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(scrollArea);
    setLayout(outerLayout);
}

void DashboardWidget::loadStatistics()
{
    updateQuickStats();
    createSkillDistributionChart();
    createRecentActivityFeed();

    QString timestamp = QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP");
    lastUpdateLabel_->setText("Last updated: " + timestamp);
}

void DashboardWidget::updateQuickStats()
{
    int engineerCount = engineerRepo_.findAll().size();
    int areaCount = productionRepo_.findAllAreas().size();

    engineerCountLabel_->setText(QString::number(engineerCount));
    productionAreaCountLabel_->setText(QString::number(areaCount));

    Logger::instance().info("DashboardWidget",
        QString("Statistics: %1 engineers, %2 areas")
        .arg(engineerCount).arg(areaCount));
}

void DashboardWidget::createSkillDistributionChart()
{
    // Get all assessments and count by score
    QList<Assessment> assessments = assessmentRepo_.findAll();

    QMap<int, int> scoreCounts;
    scoreCounts[0] = 0;  // Not assessed
    scoreCounts[1] = 0;  // Basic
    scoreCounts[2] = 0;  // Intermediate
    scoreCounts[3] = 0;  // Advanced

    for (const Assessment& assessment : assessments) {
        int score = assessment.score();
        if (score >= 0 && score <= 3) {
            scoreCounts[score]++;
        }
    }

    // Create bar chart
    skillChart_->removeAllSeries();

    QBarSet* set = new QBarSet("Assessments");
    set->setColor(StyleManager::instance().getColor("primary"));

    *set << scoreCounts[0] << scoreCounts[1] << scoreCounts[2] << scoreCounts[3];

    QBarSeries* series = new QBarSeries();
    series->append(set);

    skillChart_->addSeries(series);

    // Setup axes
    QStringList categories;
    categories << "Not Assessed\n(0)" << "Basic\n(1)" << "Intermediate\n(2)" << "Advanced\n(3)";

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    skillChart_->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Number of Assessments");
    axisY->setLabelFormat("%d");
    skillChart_->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    skillChart_->legend()->setVisible(false);
}

void DashboardWidget::createRecentActivityFeed()
{
    recentActivityList_->clear();

    // Get recent assessments (limit to 10)
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Sort by most recent (assuming we have timestamps)
    // For now, just show the last 10
    int count = qMin(10, assessments.size());

    if (count == 0) {
        QListWidgetItem* item = new QListWidgetItem("No recent activity");
        item->setForeground(StyleManager::instance().getColor("textSecondary"));
        recentActivityList_->addItem(item);
    } else {
        for (int i = assessments.size() - 1; i >= qMax(0, assessments.size() - count); --i) {
            const Assessment& assessment = assessments[i];

            // Get engineer name
            Engineer engineer = engineerRepo_.findById(assessment.engineerId());

            QString activityText;
            if (!engineer.name().isEmpty()) {
                activityText = QString("✓ %1 assessed - Score: %2")
                    .arg(engineer.name())
                    .arg(assessment.score());
            } else {
                activityText = QString("✓ Assessment completed - Score: %1")
                    .arg(assessment.score());
            }

            QListWidgetItem* item = new QListWidgetItem(
                IconProvider::instance().getIcon(IconProvider::Assessment),
                activityText
            );

            // Color code by score
            if (assessment.score() == 3) {
                item->setForeground(StyleManager::instance().getColor("accent"));
            } else if (assessment.score() >= 1) {
                item->setForeground(StyleManager::instance().getColor("primary"));
            } else {
                item->setForeground(StyleManager::instance().getColor("textSecondary"));
            }

            recentActivityList_->addItem(item);
        }
    }
}

void DashboardWidget::onRefreshClicked()
{
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Statistics refreshed by user");
}
