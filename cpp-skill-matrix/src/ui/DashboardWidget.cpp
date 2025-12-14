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
    , productionAreaCountLabel_(nullptr)
    , assessmentCountLabel_(nullptr)
    , coreSkillCountLabel_(nullptr)
    , lastUpdateLabel_(nullptr)
    , lastLoginLabel_(nullptr)
    , skillChartView_(nullptr)
    , skillChart_(nullptr)
    , recentActivityList_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Dashboard widget initialized with enhanced features");
}

DashboardWidget::~DashboardWidget()
{
}

void DashboardWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(48);  // Much larger spacing between sections
    mainLayout->setContentsMargins(48, 48, 48, 48);  // Much larger container padding

    // Title and Welcome Section
    QLabel* titleLabel = new QLabel("📊 Dashboard Overview", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(26);  // Increased from 24
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + "; margin-bottom: 8px;");
    mainLayout->addWidget(titleLabel);

    // Welcome message with user info
    Session* session = Application::instance().session();
    QString welcomeText = QString("Welcome back, %1!").arg(
        session && !session->username().isEmpty() ? session->username() : "User"
    );
    QLabel* welcomeLabel = new QLabel(welcomeText, this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(14);  // Increased from 12
    welcomeLabel->setFont(welcomeFont);
    mainLayout->addWidget(welcomeLabel);

    // Last login time
    lastLoginLabel_ = new QLabel(
        QString("Last login: %1").arg(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP")),
        this
    );
    lastLoginLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    QFont lastLoginFont = lastLoginLabel_->font();
    lastLoginFont.setPointSize(12);  // Added explicit font size
    lastLoginLabel_->setFont(lastLoginFont);
    mainLayout->addWidget(lastLoginLabel_);

    mainLayout->addSpacing(48);  // Much larger gap after welcome section

    // Quick Statistics Grid (2x2 cards)
    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(32);  // Much larger spacing between stat cards
    statsGrid->setContentsMargins(0, 0, 0, 0);

    // Engineers Card
    QGroupBox* engineersBox = new QGroupBox("👥 Total Engineers", this);
    QVBoxLayout* engineersLayout = new QVBoxLayout(engineersBox);
    engineersLayout->setSpacing(8);  // gap-2 internal spacing
    engineersLayout->setContentsMargins(20, 20, 20, 20);  // p-5 stat card padding
    engineerCountLabel_ = new QLabel("0", this);
    QFont countFont = engineerCountLabel_->font();
    countFont.setPointSize(40);  // Increased from 36
    countFont.setBold(true);
    engineerCountLabel_->setFont(countFont);
    engineerCountLabel_->setAlignment(Qt::AlignCenter);
    engineerCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + "; margin: 8px 0;");
    engineersLayout->addWidget(engineerCountLabel_);
    QLabel* engineersSubtext = new QLabel("Active workforce", this);
    engineersSubtext->setAlignment(Qt::AlignCenter);
    QFont subtextFont = engineersSubtext->font();
    subtextFont.setPointSize(12);  // Added explicit font size
    engineersSubtext->setFont(subtextFont);
    engineersSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    engineersLayout->addWidget(engineersSubtext);
    engineersBox->setMinimumHeight(180);  // Increased from 140

    // Production Areas Card
    QGroupBox* areasBox = new QGroupBox("🏭 Production Areas", this);
    QVBoxLayout* areasLayout = new QVBoxLayout(areasBox);
    areasLayout->setSpacing(8);  // gap-2 internal spacing
    areasLayout->setContentsMargins(20, 20, 20, 20);  // p-5 stat card padding
    productionAreaCountLabel_ = new QLabel("0", this);
    productionAreaCountLabel_->setFont(countFont);
    productionAreaCountLabel_->setAlignment(Qt::AlignCenter);
    productionAreaCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("accent").name() + "; margin: 8px 0;");
    areasLayout->addWidget(productionAreaCountLabel_);
    QLabel* areasSubtext = new QLabel("Defined work areas", this);
    areasSubtext->setFont(subtextFont);
    areasSubtext->setAlignment(Qt::AlignCenter);
    areasSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    areasLayout->addWidget(areasSubtext);
    areasBox->setMinimumHeight(180);  // Increased from 140

    // Assessments Card
    QGroupBox* assessmentsBox = new QGroupBox("📋 Total Assessments", this);
    QVBoxLayout* assessmentsLayout = new QVBoxLayout(assessmentsBox);
    assessmentsLayout->setSpacing(8);  // gap-2 internal spacing
    assessmentsLayout->setContentsMargins(20, 20, 20, 20);  // p-5 stat card padding
    assessmentCountLabel_ = new QLabel("0", this);
    assessmentCountLabel_->setFont(countFont);
    assessmentCountLabel_->setAlignment(Qt::AlignCenter);
    assessmentCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("warning").name() + "; margin: 8px 0;");
    assessmentsLayout->addWidget(assessmentCountLabel_);
    QLabel* assessmentsSubtext = new QLabel("Competency evaluations", this);
    assessmentsSubtext->setFont(subtextFont);
    assessmentsSubtext->setAlignment(Qt::AlignCenter);
    assessmentsSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    assessmentsLayout->addWidget(assessmentsSubtext);
    assessmentsBox->setMinimumHeight(180);  // Increased from 140

    // Core Skills Card
    QGroupBox* coreSkillsBox = new QGroupBox("💡 Core Skills Assessed", this);
    QVBoxLayout* coreSkillsLayout = new QVBoxLayout(coreSkillsBox);
    coreSkillsLayout->setSpacing(8);  // gap-2 internal spacing
    coreSkillsLayout->setContentsMargins(20, 20, 20, 20);  // p-5 stat card padding
    coreSkillCountLabel_ = new QLabel("0", this);
    coreSkillCountLabel_->setFont(countFont);
    coreSkillCountLabel_->setAlignment(Qt::AlignCenter);
    coreSkillCountLabel_->setStyleSheet("color: #E91E63; margin: 8px 0;");
    coreSkillsLayout->addWidget(coreSkillCountLabel_);
    QLabel* coreSkillsSubtext = new QLabel("Fundamental competencies", this);
    coreSkillsSubtext->setFont(subtextFont);
    coreSkillsSubtext->setAlignment(Qt::AlignCenter);
    coreSkillsSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    coreSkillsLayout->addWidget(coreSkillsSubtext);
    coreSkillsBox->setMinimumHeight(180);  // Increased from 140

    statsGrid->addWidget(engineersBox, 0, 0);
    statsGrid->addWidget(areasBox, 0, 1);
    statsGrid->addWidget(assessmentsBox, 1, 0);
    statsGrid->addWidget(coreSkillsBox, 1, 1);

    mainLayout->addLayout(statsGrid);

    mainLayout->addSpacing(48);  // Much larger spacing before charts

    // Charts and Activity Section
    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(32);  // Much larger spacing between chart panels

    // Skill Distribution Chart
    QGroupBox* chartBox = new QGroupBox("📊 Skill Level Distribution", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartBox);
    chartLayout->setContentsMargins(16, 16, 16, 16);  // p-4 smaller panels

    skillChart_ = new QChart();
    skillChart_->setTitle("");
    skillChart_->setAnimationOptions(QChart::SeriesAnimations);
    skillChart_->legend()->setAlignment(Qt::AlignBottom);
    skillChart_->setMargins(QMargins(12, 12, 12, 12));  // Added chart margins

    skillChartView_ = new QChartView(skillChart_, this);
    skillChartView_->setRenderHint(QPainter::Antialiasing);
    skillChartView_->setMinimumHeight(350);  // Increased from 300

    chartLayout->addWidget(skillChartView_);
    chartBox->setMinimumWidth(450);  // Increased from 400

    // Recent Activity Feed
    QGroupBox* activityBox = new QGroupBox("📝 Recent Activity", this);
    QVBoxLayout* activityLayout = new QVBoxLayout(activityBox);
    activityLayout->setContentsMargins(16, 16, 16, 16);  // p-4 smaller panels

    recentActivityList_ = new QListWidget(this);
    recentActivityList_->setMinimumHeight(350);  // Increased from 300
    recentActivityList_->setAlternatingRowColors(true);

    activityLayout->addWidget(recentActivityList_);
    activityBox->setMinimumWidth(400);  // Increased from 350

    chartsLayout->addWidget(chartBox, 6);
    chartsLayout->addWidget(activityBox, 4);

    mainLayout->addLayout(chartsLayout);

    mainLayout->addSpacing(48);  // Much larger spacing before actions

    // Quick Actions Guide
    QGroupBox* actionsBox = new QGroupBox("🚀 Quick Actions", this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsBox);
    actionsLayout->setContentsMargins(24, 24, 24, 24);  // p-6 large card padding

    QLabel* actionsLabel = new QLabel(
        "<ul style='line-height: 2.0;'>"  // Increased from 1.6
        "<li style='margin-bottom: 8px;'><b>Engineers:</b> Manage engineer profiles and information</li>"
        "<li style='margin-bottom: 8px;'><b>Production Areas:</b> Define areas, machines, and competencies</li>"
        "<li style='margin-bottom: 8px;'><b>Assessments:</b> Perform and track competency evaluations</li>"
        "<li style='margin-bottom: 8px;'><b>Reports:</b> Generate detailed skill matrix reports</li>"
        "<li style='margin-bottom: 8px;'><b>Analytics:</b> View charts and insights</li>"
        "<li style='margin-bottom: 8px;'><b>Import/Export:</b> Transfer data to/from external systems</li>"
        "</ul>", this);
    actionsLabel->setTextFormat(Qt::RichText);
    actionsLabel->setWordWrap(true);
    QFont actionsFont = actionsLabel->font();
    actionsFont.setPointSize(13);  // Added explicit font size
    actionsLabel->setFont(actionsFont);

    actionsLayout->addWidget(actionsLabel);
    mainLayout->addWidget(actionsBox);

    mainLayout->addSpacing(32);  // Larger spacing before footer

    // Footer with last update and refresh button
    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 12, 0, 0);  // Added top margin

    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    lastUpdateLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    QFont footerFont = lastUpdateLabel_->font();
    footerFont.setPointSize(12);  // Added explicit font size
    lastUpdateLabel_->setFont(footerFont);
    footerLayout->addWidget(lastUpdateLabel_);

    footerLayout->addStretch();

    refreshButton_ = new QPushButton(IconProvider::instance().getIcon(IconProvider::Refresh),
                                      "Refresh Statistics", this);
    refreshButton_->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));
    connect(refreshButton_, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    footerLayout->addWidget(refreshButton_);

    mainLayout->addLayout(footerLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
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
    int assessmentCount = assessmentRepo_.findAll().size();
    int coreSkillCount = coreSkillsRepo_.findAllAssessments().size();

    engineerCountLabel_->setText(QString::number(engineerCount));
    productionAreaCountLabel_->setText(QString::number(areaCount));
    assessmentCountLabel_->setText(QString::number(assessmentCount));
    coreSkillCountLabel_->setText(QString::number(coreSkillCount));

    Logger::instance().info("DashboardWidget",
        QString("Statistics: %1 engineers, %2 areas, %3 assessments, %4 core skills")
        .arg(engineerCount).arg(areaCount).arg(assessmentCount).arg(coreSkillCount));
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
