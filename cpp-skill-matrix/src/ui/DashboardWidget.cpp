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

using namespace QtCharts;

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
    mainLayout->setSpacing(16);

    // Title and Welcome Section
    QLabel* titleLabel = new QLabel("📊 Dashboard Overview", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    mainLayout->addWidget(titleLabel);

    // Welcome message with user info
    Session* session = Application::instance().session();
    QString welcomeText = QString("Welcome back, %1!").arg(
        session && !session->username().isEmpty() ? session->username() : "User"
    );
    QLabel* welcomeLabel = new QLabel(welcomeText, this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(12);
    welcomeLabel->setFont(welcomeFont);
    mainLayout->addWidget(welcomeLabel);

    // Last login time
    lastLoginLabel_ = new QLabel(
        QString("Last login: %1").arg(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP")),
        this
    );
    lastLoginLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    mainLayout->addWidget(lastLoginLabel_);

    mainLayout->addSpacing(10);

    // Quick Statistics Grid (2x2 cards)
    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(16);

    // Engineers Card
    QGroupBox* engineersBox = new QGroupBox("👥 Total Engineers", this);
    QVBoxLayout* engineersLayout = new QVBoxLayout(engineersBox);
    engineerCountLabel_ = new QLabel("0", this);
    QFont countFont = engineerCountLabel_->font();
    countFont.setPointSize(36);
    countFont.setBold(true);
    engineerCountLabel_->setFont(countFont);
    engineerCountLabel_->setAlignment(Qt::AlignCenter);
    engineerCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    engineersLayout->addWidget(engineerCountLabel_);
    QLabel* engineersSubtext = new QLabel("Active workforce", this);
    engineersSubtext->setAlignment(Qt::AlignCenter);
    engineersSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    engineersLayout->addWidget(engineersSubtext);
    engineersBox->setMinimumHeight(140);

    // Production Areas Card
    QGroupBox* areasBox = new QGroupBox("🏭 Production Areas", this);
    QVBoxLayout* areasLayout = new QVBoxLayout(areasBox);
    productionAreaCountLabel_ = new QLabel("0", this);
    productionAreaCountLabel_->setFont(countFont);
    productionAreaCountLabel_->setAlignment(Qt::AlignCenter);
    productionAreaCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("accent").name() + ";");
    areasLayout->addWidget(productionAreaCountLabel_);
    QLabel* areasSubtext = new QLabel("Defined work areas", this);
    areasSubtext->setAlignment(Qt::AlignCenter);
    areasSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    areasLayout->addWidget(areasSubtext);
    areasBox->setMinimumHeight(140);

    // Assessments Card
    QGroupBox* assessmentsBox = new QGroupBox("📋 Total Assessments", this);
    QVBoxLayout* assessmentsLayout = new QVBoxLayout(assessmentsBox);
    assessmentCountLabel_ = new QLabel("0", this);
    assessmentCountLabel_->setFont(countFont);
    assessmentCountLabel_->setAlignment(Qt::AlignCenter);
    assessmentCountLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("warning").name() + ";");
    assessmentsLayout->addWidget(assessmentCountLabel_);
    QLabel* assessmentsSubtext = new QLabel("Competency evaluations", this);
    assessmentsSubtext->setAlignment(Qt::AlignCenter);
    assessmentsSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    assessmentsLayout->addWidget(assessmentsSubtext);
    assessmentsBox->setMinimumHeight(140);

    // Core Skills Card
    QGroupBox* coreSkillsBox = new QGroupBox("💡 Core Skills Assessed", this);
    QVBoxLayout* coreSkillsLayout = new QVBoxLayout(coreSkillsBox);
    coreSkillCountLabel_ = new QLabel("0", this);
    coreSkillCountLabel_->setFont(countFont);
    coreSkillCountLabel_->setAlignment(Qt::AlignCenter);
    coreSkillCountLabel_->setStyleSheet("color: #E91E63;");
    coreSkillsLayout->addWidget(coreSkillCountLabel_);
    QLabel* coreSkillsSubtext = new QLabel("Fundamental competencies", this);
    coreSkillsSubtext->setAlignment(Qt::AlignCenter);
    coreSkillsSubtext->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    coreSkillsLayout->addWidget(coreSkillsSubtext);
    coreSkillsBox->setMinimumHeight(140);

    statsGrid->addWidget(engineersBox, 0, 0);
    statsGrid->addWidget(areasBox, 0, 1);
    statsGrid->addWidget(assessmentsBox, 1, 0);
    statsGrid->addWidget(coreSkillsBox, 1, 1);

    mainLayout->addLayout(statsGrid);

    // Charts and Activity Section
    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(16);

    // Skill Distribution Chart
    QGroupBox* chartBox = new QGroupBox("📊 Skill Level Distribution", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartBox);

    skillChart_ = new QChart();
    skillChart_->setTitle("");
    skillChart_->setAnimationOptions(QChart::SeriesAnimations);
    skillChart_->legend()->setAlignment(Qt::AlignBottom);

    skillChartView_ = new QChartView(skillChart_, this);
    skillChartView_->setRenderHint(QPainter::Antialiasing);
    skillChartView_->setMinimumHeight(300);

    chartLayout->addWidget(skillChartView_);
    chartBox->setMinimumWidth(400);

    // Recent Activity Feed
    QGroupBox* activityBox = new QGroupBox("📝 Recent Activity", this);
    QVBoxLayout* activityLayout = new QVBoxLayout(activityBox);

    recentActivityList_ = new QListWidget(this);
    recentActivityList_->setMinimumHeight(300);
    recentActivityList_->setAlternatingRowColors(true);

    activityLayout->addWidget(recentActivityList_);
    activityBox->setMinimumWidth(350);

    chartsLayout->addWidget(chartBox, 6);
    chartsLayout->addWidget(activityBox, 4);

    mainLayout->addLayout(chartsLayout);

    // Quick Actions Guide
    QGroupBox* actionsBox = new QGroupBox("🚀 Quick Actions", this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsBox);

    QLabel* actionsLabel = new QLabel(
        "<ul style='line-height: 1.6;'>"
        "<li><b>Engineers:</b> Manage engineer profiles and information</li>"
        "<li><b>Production Areas:</b> Define areas, machines, and competencies</li>"
        "<li><b>Assessments:</b> Perform and track competency evaluations</li>"
        "<li><b>Reports:</b> Generate detailed skill matrix reports</li>"
        "<li><b>Analytics:</b> View charts and insights</li>"
        "<li><b>Import/Export:</b> Transfer data to/from external systems</li>"
        "</ul>", this);
    actionsLabel->setTextFormat(Qt::RichText);
    actionsLabel->setWordWrap(true);

    actionsLayout->addWidget(actionsLabel);
    mainLayout->addWidget(actionsBox);

    // Footer with last update and refresh button
    QHBoxLayout* footerLayout = new QHBoxLayout();

    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    lastUpdateLabel_->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
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
