#include "DashboardWidget.h"
#include "StyleManager.h"
#include "AptitudeLogoWidget.h"
#include "../utils/Logger.h"
#include "../utils/IconProvider.h"
#include "../core/Session.h"
#include "../core/Application.h"
#include "../core/DataCache.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QDateTime>
#include <QScrollArea>
#include <QProgressBar>

#include <algorithm>

DashboardWidget::DashboardWidget(QWidget* parent)
    : QWidget(parent)
    , criticalAlertsList_(nullptr)
    , machinesAtRiskLabel_(nullptr)
    , criticalCompetenciesLabel_(nullptr)
    , trainingInProgressLabel_(nullptr)
    , promotionReadyLabel_(nullptr)
    , machineReadinessContainer_(nullptr)
    , shiftComparisonContainer_(nullptr)
    , urgentTrainingList_(nullptr)
    , promotionReadyList_(nullptr)
    , recentActivityList_(nullptr)
    , viewAnalyticsButton_(nullptr)
    , scheduleTrainingButton_(nullptr)
    , assessEngineersButton_(nullptr)
    , exportReportButton_(nullptr)
    , lastUpdateLabel_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Dashboard widget initialized with actionable insights");
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
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Header with logo
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(20);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("Command Center", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("Your action plan for today - prioritized by business impact", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #64748b;");
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    AptitudeLogoWidget* logoWidget = new AptitudeLogoWidget(this);
    logoWidget->setSize(120);
    headerLayout->addWidget(logoWidget);

    mainLayout->addLayout(headerLayout);

    // Refresh button in header
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
    connect(refreshButton, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton);

    mainLayout->addSpacing(16);

    // Section 1: Critical Alerts
    setupCriticalAlertsSection(mainLayout);

    // Section 2: Production Health
    setupProductionHealthSection(mainLayout);

    // Section 3: Machine Readiness
    setupMachineReadinessSection(mainLayout);

    // Section 4: Shift Comparison
    setupShiftComparisonSection(mainLayout);

    // Section 5: Training Pipeline
    setupTrainingPipelineSection(mainLayout);

    // Section 6: Recent Activity
    setupRecentActivitySection(mainLayout);

    // Section 7: Quick Actions
    setupQuickActionsSection(mainLayout);

    // Footer
    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    lastUpdateLabel_->setStyleSheet("color: #64748b;");
    QFont footerFont = lastUpdateLabel_->font();
    footerFont.setPointSize(12);
    lastUpdateLabel_->setFont(footerFont);
    mainLayout->addWidget(lastUpdateLabel_);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(scrollArea);
    setLayout(outerLayout);
}

void DashboardWidget::setupCriticalAlertsSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("🚨 CRITICAL ALERTS - Action Required", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* alertsBox = new QGroupBox(this);
    alertsBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #fee2e2;"
        "    border-left: 4px solid #ff6b6b;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "}"
    );

    QVBoxLayout* alertsLayout = new QVBoxLayout(alertsBox);
    criticalAlertsList_ = new QListWidget(this);
    criticalAlertsList_->setMinimumHeight(150);
    criticalAlertsList_->setSpacing(12);
    criticalAlertsList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item {"
        "    border-radius: 6px;"
        "    padding: 12px;"
        "    margin-bottom: 8px;"
        "    background-color: #fef3c7;"
        "}"
    );
    QFont listFont = criticalAlertsList_->font();
    listFont.setPointSize(13);
    criticalAlertsList_->setFont(listFont);
    alertsLayout->addWidget(criticalAlertsList_);

    mainLayout->addWidget(alertsBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupProductionHealthSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Production Health Overview", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGridLayout* cardsGrid = new QGridLayout();
    cardsGrid->setSpacing(16);

    auto createCard = [this](const QString& title, QLabel*& valueLabel, const QString& subtitle, const QString& borderColor) -> QWidget* {
        QGroupBox* card = new QGroupBox(this);
        card->setMinimumHeight(140);
        card->setStyleSheet(QString(
            "QGroupBox {"
            "    background-color: white;"
            "    border-left: 4px solid %1;"
            "    border-radius: 8px;"
            "    padding: 20px;"
            "}"
        ).arg(borderColor));

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(8);

        QLabel* titleLabel = new QLabel(title, this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("color: #64748b;");
        cardLayout->addWidget(titleLabel);

        valueLabel = new QLabel("0", this);
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(42);
        valueFont.setBold(true);
        valueLabel->setFont(valueFont);
        valueLabel->setStyleSheet("color: #1e293b;");
        cardLayout->addWidget(valueLabel);

        QLabel* subtitleLabel = new QLabel(subtitle, this);
        QFont subFont = subtitleLabel->font();
        subFont.setPointSize(11);
        subtitleLabel->setFont(subFont);
        subtitleLabel->setStyleSheet("color: #64748b;");
        cardLayout->addWidget(subtitleLabel);

        cardLayout->addStretch();
        return card;
    };

    QWidget* card1 = createCard("MACHINES AT RISK", machinesAtRiskLabel_, "under 50% coverage", "#ff6b6b");
    QWidget* card2 = createCard("CRITICAL COMPETENCIES", criticalCompetenciesLabel_, "need more staff", "#fbbf24");
    QWidget* card3 = createCard("TRAINING IN PROGRESS", trainingInProgressLabel_, "assessments this month", "#60a5fa");
    QWidget* card4 = createCard("READY FOR PROMOTION", promotionReadyLabel_, "engineers >90%", "#4ade80");

    cardsGrid->addWidget(card1, 0, 0);
    cardsGrid->addWidget(card2, 0, 1);
    cardsGrid->addWidget(card3, 0, 2);
    cardsGrid->addWidget(card4, 0, 3);

    mainLayout->addLayout(cardsGrid);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupMachineReadinessSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Machine Coverage Status", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* readinessBox = new QGroupBox(this);
    readinessBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "}"
    );

    QVBoxLayout* readinessLayout = new QVBoxLayout(readinessBox);
    machineReadinessContainer_ = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(machineReadinessContainer_);
    containerLayout->setSpacing(12);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    readinessLayout->addWidget(machineReadinessContainer_);
    mainLayout->addWidget(readinessBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupShiftComparisonSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Shift Performance", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* shiftBox = new QGroupBox(this);
    shiftBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "}"
    );

    QVBoxLayout* shiftLayout = new QVBoxLayout(shiftBox);
    shiftComparisonContainer_ = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(shiftComparisonContainer_);
    containerLayout->setSpacing(12);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    shiftLayout->addWidget(shiftComparisonContainer_);
    mainLayout->addWidget(shiftBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupTrainingPipelineSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Training Pipeline - This Week's Focus", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QHBoxLayout* pipelineRow = new QHBoxLayout();
    pipelineRow->setSpacing(16);

    // Urgent Training
    QGroupBox* urgentBox = new QGroupBox("🎯 URGENT TRAINING", this);
    urgentBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #fee2e2;"
        "    border-radius: 8px;"
        "    padding: 16px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* urgentLayout = new QVBoxLayout(urgentBox);
    urgentTrainingList_ = new QListWidget(this);
    urgentTrainingList_->setMinimumHeight(200);
    urgentTrainingList_->setSpacing(8);
    urgentTrainingList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item {"
        "    padding: 12px;"
        "    margin-bottom: 6px;"
        "    border-radius: 6px;"
        "}"
    );
    QFont listFont = urgentTrainingList_->font();
    listFont.setPointSize(12);
    urgentTrainingList_->setFont(listFont);
    urgentLayout->addWidget(urgentTrainingList_);
    pipelineRow->addWidget(urgentBox, 1);

    // Promotion Ready
    QGroupBox* promotionBox = new QGroupBox("👤 PROMOTION READY", this);
    promotionBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #d1fae5;"
        "    border-radius: 8px;"
        "    padding: 16px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* promotionLayout = new QVBoxLayout(promotionBox);
    promotionReadyList_ = new QListWidget(this);
    promotionReadyList_->setMinimumHeight(200);
    promotionReadyList_->setSpacing(8);
    promotionReadyList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item {"
        "    padding: 12px;"
        "    margin-bottom: 6px;"
        "    border-radius: 6px;"
        "}"
    );
    promotionReadyList_->setFont(listFont);
    promotionLayout->addWidget(promotionReadyList_);
    pipelineRow->addWidget(promotionBox, 1);

    mainLayout->addLayout(pipelineRow);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupRecentActivitySection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Recent Changes (Last 7 Days)", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* activityBox = new QGroupBox(this);
    activityBox->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "}"
    );

    QVBoxLayout* activityLayout = new QVBoxLayout(activityBox);
    recentActivityList_ = new QListWidget(this);
    recentActivityList_->setMinimumHeight(150);
    recentActivityList_->setSpacing(8);
    recentActivityList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item {"
        "    padding: 10px;"
        "    margin-bottom: 6px;"
        "    border-radius: 6px;"
        "}"
    );
    QFont listFont = recentActivityList_->font();
    listFont.setPointSize(13);
    recentActivityList_->setFont(listFont);
    activityLayout->addWidget(recentActivityList_);

    mainLayout->addWidget(activityBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupQuickActionsSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Quick Actions", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QHBoxLayout* actionsRow = new QHBoxLayout();
    actionsRow->setSpacing(16);

    auto createActionButton = [this](const QString& text, const QString& color) -> QPushButton* {
        QPushButton* btn = new QPushButton(text, this);
        QFont btnFont = btn->font();
        btnFont.setPointSize(14);
        btnFont.setWeight(QFont::DemiBold);
        btn->setFont(btnFont);
        btn->setMinimumHeight(50);
        btn->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 12px 24px;"
            "}"
            "QPushButton:hover {"
            "    opacity: 0.9;"
            "}"
        ).arg(color));
        return btn;
    };

    viewAnalyticsButton_ = createActionButton("📊 View Full Analytics", "#ff6b6b");
    scheduleTrainingButton_ = createActionButton("🎯 Schedule Training", "#4ade80");
    assessEngineersButton_ = createActionButton("👥 Assess Engineers", "#60a5fa");
    exportReportButton_ = createActionButton("📋 Export Report", "#64748b");

    actionsRow->addWidget(viewAnalyticsButton_);
    actionsRow->addWidget(scheduleTrainingButton_);
    actionsRow->addWidget(assessEngineersButton_);
    actionsRow->addWidget(exportReportButton_);

    mainLayout->addLayout(actionsRow);
    mainLayout->addSpacing(24);
}

void DashboardWidget::loadStatistics()
{
    Logger::instance().info("DashboardWidget", "Loading dashboard statistics...");

    updateCriticalAlerts();
    updateProductionHealth();
    updateMachineReadiness();
    updateShiftComparison();
    updateTrainingPipeline();
    updateRecentActivity();

    QString timestamp = QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP");
    lastUpdateLabel_->setText("Last updated: " + timestamp);

    Logger::instance().info("DashboardWidget", "Dashboard statistics loaded");
}

void DashboardWidget::updateCriticalAlerts()
{
    criticalAlertsList_->clear();

    QList<MachineReadiness> machineReadiness = calculateMachineReadiness();
    QList<TrainingRecommendation> trainingPriorities = calculateTrainingPriorities();

    int alertCount = 0;
    const int maxAlerts = 5;

    // Critical machine coverage alerts
    for (const MachineReadiness& machine : machineReadiness) {
        if (alertCount >= maxAlerts) break;

        if (machine.coveragePercent < 30) {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("⚠️ %1: Only %2% coverage\n   Action: URGENT - Train %3 engineers to reach 70%% coverage")
                .arg(machine.machineName)
                .arg(QString::number(machine.coveragePercent, 'f', 0))
                .arg(qMax(1, int((machine.totalEngineers * 0.7) - machine.proficientCount)));
            item->setText(text);
            item->setBackground(QBrush(QColor("#fee2e2")));
            item->setForeground(QBrush(QColor("#1e293b")));
            criticalAlertsList_->addItem(item);
            alertCount++;
        } else if (machine.expertCount <= 1 && machine.importance >= 2) {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("⚠️ %1: Single point of failure (%2 expert%3)\n   Action: Cross-train 2 backup engineers immediately")
                .arg(machine.machineName)
                .arg(machine.expertCount)
                .arg(machine.expertCount == 1 ? "" : "s");
            item->setText(text);
            item->setBackground(QBrush(QColor("#fef3c7")));
            item->setForeground(QBrush(QColor("#1e293b")));
            criticalAlertsList_->addItem(item);
            alertCount++;
        }
    }

    // High impact training opportunities
    if (alertCount < maxAlerts && !trainingPriorities.isEmpty()) {
        const TrainingRecommendation& top = trainingPriorities.first();
        if (top.priority == "urgent") {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("🎯 HIGH IMPACT: Train %1 engineers in '%2'\n   Impact: %3/10 - This will significantly improve production capability")
                .arg(top.engineersNeed)
                .arg(top.competencyName)
                .arg(QString::number(top.impactScore, 'f', 1));
            item->setText(text);
            item->setBackground(QBrush(QColor("#e0f2fe")));
            item->setForeground(QBrush(QColor("#1e293b")));
            criticalAlertsList_->addItem(item);
        }
    }

    if (criticalAlertsList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
        item->setText("✅ No Critical Alerts\nAll systems operating within safe parameters!");
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));
        QFont font = item->font();
        font.setPointSize(14);
        font.setBold(true);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void DashboardWidget::updateProductionHealth()
{
    QList<MachineReadiness> machineReadiness = calculateMachineReadiness();
    QList<TrainingRecommendation> trainingPriorities = calculateTrainingPriorities();

    // Card 1: Machines at risk (< 50% coverage)
    int machinesAtRisk = 0;
    for (const MachineReadiness& machine : machineReadiness) {
        if (machine.coveragePercent < 50) {
            machinesAtRisk++;
        }
    }
    machinesAtRiskLabel_->setText(QString::number(machinesAtRisk));

    // Card 2: Critical competencies (urgent training needs)
    int criticalCompetencies = 0;
    for (const TrainingRecommendation& rec : trainingPriorities) {
        if (rec.priority == "urgent") {
            criticalCompetencies++;
        }
    }
    criticalCompetenciesLabel_->setText(QString::number(criticalCompetencies));

    // Card 3: Training in progress (assessments in last 30 days)
    QList<Assessment> allAssessments = assessmentRepo_.findAll();
    int recentAssessments = 0;
    QDateTime thirtyDaysAgo = QDateTime::currentDateTime().addDays(-30);
    for (const Assessment& assessment : allAssessments) {
        if (assessment.updatedAt() >= thirtyDaysAgo) {
            recentAssessments++;
        }
    }
    trainingInProgressLabel_->setText(QString::number(recentAssessments));

    // Card 4: Ready for promotion (>90% proficiency)
    QList<Engineer> engineers = engineerRepo_.findAll();
    int promotionReady = 0;
    for (const Engineer& engineer : engineers) {
        int totalScore = 0;
        int maxScore = 0;
        for (const Assessment& assessment : allAssessments) {
            if (assessment.engineerId() == engineer.id()) {
                totalScore += assessment.score();
                maxScore += 3;
            }
        }
        if (maxScore > 0) {
            double proficiency = (totalScore * 100.0) / maxScore;
            if (proficiency >= 90.0) {
                promotionReady++;
            }
        }
    }
    promotionReadyLabel_->setText(QString::number(promotionReady));
}

void DashboardWidget::updateMachineReadiness()
{
    // Clear existing widgets
    QLayout* layout = machineReadinessContainer_->layout();
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    QList<MachineReadiness> machineReadiness = calculateMachineReadiness();

    // Sort by coverage (worst first)
    std::sort(machineReadiness.begin(), machineReadiness.end(),
              [](const MachineReadiness& a, const MachineReadiness& b) {
                  return a.coveragePercent < b.coveragePercent;
              });

    // Show top 8 machines (worst coverage)
    int count = 0;
    for (const MachineReadiness& machine : machineReadiness) {
        if (count >= 8) break;

        QWidget* row = new QWidget(this);
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(12);

        // Machine name
        QString icon;
        if (machine.coveragePercent < 30) icon = "🚨";
        else if (machine.coveragePercent < 50) icon = "⚠️";
        else if (machine.coveragePercent < 70) icon = "ℹ️";
        else icon = "✅";

        QLabel* nameLabel = new QLabel(QString("%1 %2").arg(icon).arg(machine.machineName), this);
        QFont nameFont = nameLabel->font();
        nameFont.setPointSize(13);
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        nameLabel->setMinimumWidth(200);
        rowLayout->addWidget(nameLabel);

        // Progress bar
        QProgressBar* progressBar = new QProgressBar(this);
        progressBar->setMinimum(0);
        progressBar->setMaximum(100);
        progressBar->setValue(static_cast<int>(machine.coveragePercent));
        progressBar->setTextVisible(true);
        progressBar->setFormat(QString("%1%").arg(QString::number(machine.coveragePercent, 'f', 0)));
        progressBar->setMinimumWidth(300);

        QString barColor;
        if (machine.coveragePercent < 30) barColor = "#ff6b6b";
        else if (machine.coveragePercent < 50) barColor = "#fbbf24";
        else if (machine.coveragePercent < 70) barColor = "#60a5fa";
        else barColor = "#4ade80";

        progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: 1px solid #e2e8f0;"
            "    border-radius: 4px;"
            "    text-align: center;"
            "    background-color: #f1f5f9;"
            "    color: #1e293b;"
            "    font-weight: bold;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 3px;"
            "}"
        ).arg(barColor));

        rowLayout->addWidget(progressBar);

        // Coverage stats
        QLabel* statsLabel = new QLabel(QString("%1/%2 proficient, %3 experts")
            .arg(machine.proficientCount)
            .arg(machine.totalEngineers)
            .arg(machine.expertCount), this);
        QFont statsFont = statsLabel->font();
        statsFont.setPointSize(12);
        statsLabel->setFont(statsFont);
        statsLabel->setStyleSheet("color: #64748b;");
        statsLabel->setMinimumWidth(200);
        rowLayout->addWidget(statsLabel);

        rowLayout->addStretch();

        machineReadinessContainer_->layout()->addWidget(row);
        count++;
    }
}

void DashboardWidget::updateShiftComparison()
{
    // Clear existing widgets
    QLayout* layout = shiftComparisonContainer_->layout();
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    QList<ShiftPerformance> shiftPerformance = calculateShiftPerformance();

    // Sort by performance (best first)
    std::sort(shiftPerformance.begin(), shiftPerformance.end(),
              [](const ShiftPerformance& a, const ShiftPerformance& b) {
                  return a.averageCompletion > b.averageCompletion;
              });

    for (int i = 0; i < shiftPerformance.size(); i++) {
        const ShiftPerformance& shift = shiftPerformance[i];

        QWidget* row = new QWidget(this);
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(12);

        // Shift name with trophy for best
        QString icon = (i == 0) ? "🏆 " : "";
        QLabel* nameLabel = new QLabel(QString("%1%2").arg(icon).arg(shift.shiftName), this);
        QFont nameFont = nameLabel->font();
        nameFont.setPointSize(14);
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        nameLabel->setMinimumWidth(120);
        rowLayout->addWidget(nameLabel);

        // Progress bar
        QProgressBar* progressBar = new QProgressBar(this);
        progressBar->setMinimum(0);
        progressBar->setMaximum(100);
        progressBar->setValue(static_cast<int>(shift.averageCompletion));
        progressBar->setTextVisible(true);
        progressBar->setFormat(QString("%1%").arg(QString::number(shift.averageCompletion, 'f', 0)));
        progressBar->setMinimumWidth(400);

        QString barColor = (i == 0) ? "#4ade80" : (i == shiftPerformance.size() - 1) ? "#fbbf24" : "#60a5fa";

        progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: 1px solid #e2e8f0;"
            "    border-radius: 4px;"
            "    text-align: center;"
            "    background-color: #f1f5f9;"
            "    color: #1e293b;"
            "    font-weight: bold;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 3px;"
            "}"
        ).arg(barColor));

        rowLayout->addWidget(progressBar);

        // Engineer count
        QLabel* statsLabel = new QLabel(QString("%1 engineers").arg(shift.engineerCount), this);
        QFont statsFont = statsLabel->font();
        statsFont.setPointSize(12);
        statsLabel->setFont(statsFont);
        statsLabel->setStyleSheet("color: #64748b;");
        statsLabel->setMinimumWidth(120);
        rowLayout->addWidget(statsLabel);

        rowLayout->addStretch();

        shiftComparisonContainer_->layout()->addWidget(row);
    }

    // Add gap analysis
    if (shiftPerformance.size() > 1) {
        double gap = shiftPerformance.first().averageCompletion - shiftPerformance.last().averageCompletion;
        if (gap > 10.0) {
            QLabel* gapLabel = new QLabel(QString("\n⚠️ Gap: %1% between best and worst shifts - Consider cross-shift training")
                .arg(QString::number(gap, 'f', 0)), this);
            QFont font = gapLabel->font();
            font.setPointSize(12);
            font.setBold(true);
            gapLabel->setFont(font);
            gapLabel->setStyleSheet("color: #fbbf24; padding: 10px;");
            shiftComparisonContainer_->layout()->addWidget(gapLabel);
        }
    }
}

void DashboardWidget::updateTrainingPipeline()
{
    urgentTrainingList_->clear();
    promotionReadyList_->clear();

    QList<TrainingRecommendation> trainingPriorities = calculateTrainingPriorities();

    // Urgent Training (top 5)
    int count = 0;
    for (const TrainingRecommendation& rec : trainingPriorities) {
        if (count >= 5) break;
        if (rec.priority == "urgent" || rec.priority == "high") {
            QListWidgetItem* item = new QListWidgetItem(urgentTrainingList_);
            QString text = QString("• %1\n  %2 engineers need training | Impact: %3/10")
                .arg(rec.competencyName)
                .arg(rec.engineersNeed)
                .arg(QString::number(rec.impactScore, 'f', 1));
            item->setText(text);
            item->setBackground(QBrush(QColor("#fef3c7")));
            item->setForeground(QBrush(QColor("#1e293b")));
            urgentTrainingList_->addItem(item);
            count++;
        }
    }

    if (urgentTrainingList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(urgentTrainingList_);
        item->setText("✅ No urgent training needs\nAll critical competencies are well-staffed!");
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));
    }

    // Promotion Ready (engineers >90%)
    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> allAssessments = assessmentRepo_.findAll();

    struct EngineerScore {
        QString name;
        double proficiency;
    };
    QList<EngineerScore> topEngineers;

    for (const Engineer& engineer : engineers) {
        int totalScore = 0;
        int maxScore = 0;
        for (const Assessment& assessment : allAssessments) {
            if (assessment.engineerId() == engineer.id()) {
                totalScore += assessment.score();
                maxScore += 3;
            }
        }
        if (maxScore > 0) {
            double proficiency = (totalScore * 100.0) / maxScore;
            if (proficiency >= 85.0) {
                topEngineers.append({engineer.name(), proficiency});
            }
        }
    }

    std::sort(topEngineers.begin(), topEngineers.end(),
              [](const EngineerScore& a, const EngineerScore& b) {
                  return a.proficiency > b.proficiency;
              });

    for (int i = 0; i < qMin(5, topEngineers.size()); i++) {
        QListWidgetItem* item = new QListWidgetItem(promotionReadyList_);
        QString role;
        if (topEngineers[i].proficiency >= 95) role = "→ Master Trainer";
        else if (topEngineers[i].proficiency >= 92) role = "→ Lead Operator";
        else if (topEngineers[i].proficiency >= 88) role = "→ Shift Lead";
        else role = "→ Trainer";

        QString text = QString("• %1: %2%\n  %3")
            .arg(topEngineers[i].name)
            .arg(QString::number(topEngineers[i].proficiency, 'f', 0))
            .arg(role);
        item->setText(text);
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));
        promotionReadyList_->addItem(item);
    }

    if (promotionReadyList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(promotionReadyList_);
        item->setText("No engineers currently at\npromotion threshold (>90%)");
        item->setBackground(QBrush(QColor("#f1f5f9")));
        item->setForeground(QBrush(QColor("#64748b")));
    }
}

void DashboardWidget::updateRecentActivity()
{
    recentActivityList_->clear();

    QList<Assessment> allAssessments = assessmentRepo_.findAll();
    QList<Engineer> allEngineers = engineerRepo_.findAll();

    QDateTime sevenDaysAgo = QDateTime::currentDateTime().addDays(-7);

    // Count recent assessments
    int recentAssessments = 0;
    for (const Assessment& assessment : allAssessments) {
        if (assessment.updatedAt() >= sevenDaysAgo) {
            recentAssessments++;
        }
    }

    if (recentAssessments > 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText(QString("📈 %1 new assessments completed").arg(recentAssessments));
        item->setBackground(QBrush(QColor("#e0f2fe")));
        item->setForeground(QBrush(QColor("#1e293b")));
        recentActivityList_->addItem(item);
    }

    // Count new engineers
    int newEngineers = 0;
    for (const Engineer& engineer : allEngineers) {
        if (engineer.createdAt() >= sevenDaysAgo) {
            newEngineers++;
        }
    }

    if (newEngineers > 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText(QString("👤 %1 new engineer%2 onboarded")
            .arg(newEngineers)
            .arg(newEngineers == 1 ? "" : "s"));
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));
        recentActivityList_->addItem(item);
    }

    // Placeholder for improvements (would need historical data)
    QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
    item->setText("📊 Track weekly improvements in next version");
    item->setBackground(QBrush(QColor("#f1f5f9")));
    item->setForeground(QBrush(QColor("#64748b")));
    recentActivityList_->addItem(item);

    if (recentActivityList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText("No recent activity in the last 7 days");
        item->setBackground(QBrush(QColor("#f1f5f9")));
        item->setForeground(QBrush(QColor("#64748b")));
    }
}

// Calculation methods (reused from AnalyticsWidget)

QList<DashboardWidget::MachineReadiness> DashboardWidget::calculateMachineReadiness()
{
    QList<MachineReadiness> readinessList;

    DataCache& cache = DataCache::instance();
    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> assessments = assessmentRepo_.findAll();
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    for (const ProductionArea& area : areas) {
        QList<Machine> areaMachines = cache.getMachinesByArea(area.id());

        for (const Machine& machine : areaMachines) {
            MachineReadiness readiness;
            readiness.machineName = machine.name();
            readiness.machineId = machine.id();
            readiness.importance = machine.importance();
            readiness.totalEngineers = engineers.size();
            readiness.proficientCount = 0;
            readiness.expertCount = 0;

            QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());

            if (machineCompetencies.isEmpty()) {
                readiness.coveragePercent = 0;
                readiness.isCritical = true;
                readinessList.append(readiness);
                continue;
            }

            for (const Engineer& engineer : engineers) {
                int totalScore = 0;
                int maxScore = 0;

                for (const Competency& comp : machineCompetencies) {
                    for (const Assessment& assessment : assessments) {
                        if (assessment.engineerId() == engineer.id() && assessment.competencyId() == comp.id()) {
                            totalScore += assessment.score();
                            maxScore += 3;
                            break;
                        }
                    }
                }

                if (maxScore > 0) {
                    double avgScore = double(totalScore) / (maxScore / 3);
                    if (avgScore >= 2.0) {
                        readiness.proficientCount++;
                    }
                    if (avgScore >= 2.5) {
                        readiness.expertCount++;
                    }
                }
            }

            if (readiness.totalEngineers > 0) {
                readiness.coveragePercent = (double(readiness.proficientCount) / readiness.totalEngineers) * 100.0;
            } else {
                readiness.coveragePercent = 0;
            }

            readiness.isCritical = readiness.coveragePercent < 50;
            readinessList.append(readiness);
        }
    }

    return readinessList;
}

QList<DashboardWidget::TrainingRecommendation> DashboardWidget::calculateTrainingPriorities()
{
    QList<TrainingRecommendation> recommendations;

    DataCache& cache = DataCache::instance();
    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> assessments = assessmentRepo_.findAll();
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    QList<Competency> allCompetencies;
    for (const ProductionArea& area : areas) {
        QList<Machine> areaMachines = cache.getMachinesByArea(area.id());
        for (const Machine& machine : areaMachines) {
            QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());
            allCompetencies.append(machineCompetencies);
        }
    }

    for (const Competency& comp : allCompetencies) {
        int totalScore = 0;
        int assessmentCount = 0;
        int needTraining = 0;

        for (const Assessment& assessment : assessments) {
            if (assessment.competencyId() == comp.id()) {
                totalScore += assessment.score();
                assessmentCount++;
                if (assessment.score() < 2) {
                    needTraining++;
                }
            }
        }

        if (assessmentCount > 0) {
            double avgScore = double(totalScore) / assessmentCount;
            double proficiency = (avgScore / 3.0) * 100.0;
            double importance = comp.calculatedWeight();

            if (importance >= 2.5 && proficiency < 50) {
                TrainingRecommendation rec;
                rec.competencyName = comp.name();
                rec.engineersNeed = needTraining;

                double engineerFactor = engineers.isEmpty() ? 0 : (double(needTraining) / engineers.size()) * 5.0;
                rec.impactScore = importance + engineerFactor;
                rec.impactScore = qMin(rec.impactScore, 10.0);

                if (rec.impactScore >= 7.0) {
                    rec.priority = "urgent";
                } else if (rec.impactScore >= 5.0) {
                    rec.priority = "high";
                } else {
                    rec.priority = "medium";
                }

                recommendations.append(rec);
            }
        }
    }

    std::sort(recommendations.begin(), recommendations.end(),
              [](const TrainingRecommendation& a, const TrainingRecommendation& b) {
                  return a.impactScore > b.impactScore;
              });

    return recommendations;
}

QList<DashboardWidget::ShiftPerformance> DashboardWidget::calculateShiftPerformance()
{
    QList<ShiftPerformance> shiftPerformance;

    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> assessments = assessmentRepo_.findAll();

    QMap<QString, ShiftPerformance> shiftsMap;

    for (const Engineer& engineer : engineers) {
        QString shift = engineer.shift();
        if (shift.isEmpty()) shift = "Unassigned";

        if (!shiftsMap.contains(shift)) {
            ShiftPerformance perf;
            perf.shiftName = shift;
            perf.averageCompletion = 0.0;
            perf.engineerCount = 0;
            shiftsMap[shift] = perf;
        }

        shiftsMap[shift].engineerCount++;

        int totalScore = 0;
        int maxScore = 0;
        for (const Assessment& assessment : assessments) {
            if (assessment.engineerId() == engineer.id()) {
                totalScore += assessment.score();
                maxScore += 3;
            }
        }

        if (maxScore > 0) {
            shiftsMap[shift].averageCompletion += (totalScore * 100.0) / maxScore;
        }
    }

    for (auto it = shiftsMap.begin(); it != shiftsMap.end(); ++it) {
        ShiftPerformance perf = it.value();
        if (perf.engineerCount > 0) {
            perf.averageCompletion /= perf.engineerCount;
        }
        shiftPerformance.append(perf);
    }

    return shiftPerformance;
}

void DashboardWidget::onRefreshClicked()
{
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Dashboard refreshed");
}
