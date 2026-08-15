#include "AnalyticsWidget.h"
#include "../utils/Logger.h"
#include "../core/DataCache.h"
#include "EngineerDevelopmentWidget.h"
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
#include <QtCharts/QScatterSeries>
#include <QLinearGradient>
#include <QProgressBar>
#include <QDateTime>
#include <QSet>
#include <QTimer>
#include <algorithm>

namespace {
bool containsAnyToken(const QString& text, const QStringList& tokens)
{
    for (const QString& token : tokens) {
        if (text.contains(token)) {
            return true;
        }
    }
    return false;
}

QString mapCoreSkillDiscipline(const QString& categoryId, const QString& categoryName,
                               const QString& skillName, const QString& storedDiscipline = QString())
{
    if (!storedDiscipline.trimmed().isEmpty()) {
        return storedDiscipline.trimmed();
    }
    const QString combined = QString("%1 %2 %3")
        .arg(categoryId.toLower(), categoryName.toLower(), skillName.toLower());

    const QStringList mechanicalTokens = {
        "mechanical", "mech", "lathe", "mill", "pump", "pneumatic", "hydraulic",
        "gearbox", "bearing", "shaft", "belt", "chain", "welding"
    };
    const QStringList electricalTokens = {
        "electrical", "elec", "motor", "control panel", "wiring", "relay", "contactor",
        "voltage", "current", "inverter", "vfd", "drive", "sensor"
    };
    const QStringList softwareTokens = {
        "software", "plc", "hmi", "scada", "logic", "program", "automation",
        "rockwell", "siemens", "tia", "studio 5000", "gx developer", "code", "rslinx"
    };

    if (containsAnyToken(combined, mechanicalTokens)) {
        return "Mechanical";
    }
    if (containsAnyToken(combined, electricalTokens)) {
        return "Electrical";
    }
    if (containsAnyToken(combined, softwareTokens)) {
        return "Software";
    }
    return "Unknown";
}
}

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
    , contentStack_(nullptr)
    , trendsButton_(nullptr)
    , shiftsButton_(nullptr)
    , insightsButton_(nullptr)
    , engineerRadarButton_(nullptr)
    , shiftOverviewButton_(nullptr)
    , criticalSkillsButton_(nullptr)
    , machineReadinessButton_(nullptr)
    , engineerDevelopmentWidget_(nullptr)
    , currentCompletionLabel_(nullptr)
    , predictedCompletionLabel_(nullptr)
    , changeLabel_(nullptr)
    , trendChartView_(nullptr)
    , shiftCardsContainer_(nullptr)
    , shiftChartView_(nullptr)
    , insightsList_(nullptr)
    , riskMatrixChartView_(nullptr)
    , trainingPriorityList_(nullptr)
    , machineReadinessAreaFilter_(nullptr)
    , machineReadinessList_(nullptr)
    , vulnerabilityList_(nullptr)
    , engineerSelector_(nullptr)
    , engineerProductionRadarView_(nullptr)
    , engineerCoreSkillsRadarView_(nullptr)
    , shiftFilterCombo_(nullptr)
    , shiftDataTypeCombo_(nullptr)
    , shiftRadarContainer_(nullptr)
    , isFirstShow_(true)
    , waitingForCacheWarmup_(false)
    , insightsLoaded_(false)
    , coreSkillsCacheLoaded_(false)
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
    machineReadinessButton_ = createTabButton("Machine Readiness");
    engineerRadarButton_ = createTabButton("Engineer Development");
    shiftOverviewButton_ = createTabButton("Shift Overview");

    tabBarLayout->addWidget(trendsButton_);
    tabBarLayout->addWidget(shiftsButton_);
    tabBarLayout->addWidget(insightsButton_);
    tabBarLayout->addWidget(machineReadinessButton_);
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
    QWidget* machineReadinessWidget = new QWidget();
    engineerDevelopmentWidget_ = new EngineerDevelopmentWidget();
    QWidget* shiftOverviewWidget = new QWidget();

    setupTrendsTab(trendsWidget);
    setupShiftComparisonTab(shiftsWidget);
    setupAutomatedInsightsTab(insightsWidget);
    setupMachineReadinessTab(machineReadinessWidget);
    setupShiftOverviewTab(shiftOverviewWidget);

    contentStack_->addWidget(trendsWidget);              // Index 0
    contentStack_->addWidget(shiftsWidget);              // Index 1
    contentStack_->addWidget(insightsWidget);            // Index 2
    contentStack_->addWidget(machineReadinessWidget);    // Index 3
    contentStack_->addWidget(engineerDevelopmentWidget_);// Index 4
    contentStack_->addWidget(shiftOverviewWidget);       // Index 5

    connect(trendsButton_, &QPushButton::clicked, [this]() { onTabChanged(0); });
    connect(shiftsButton_, &QPushButton::clicked, [this]() { onTabChanged(1); });
    connect(insightsButton_, &QPushButton::clicked, [this]() { onTabChanged(2); });
    connect(machineReadinessButton_, &QPushButton::clicked, [this]() { onTabChanged(3); });
    connect(engineerRadarButton_, &QPushButton::clicked, [this]() { onTabChanged(4); });
    connect(shiftOverviewButton_, &QPushButton::clicked, [this]() { onTabChanged(5); });

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

    QGroupBox* insightsGroup = new QGroupBox("Manager Action Insights", this);
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
    QLabel* guidance = new QLabel(
        "Prioritised from current assessment coverage. Use Engineer Development to turn an insight into an owned action plan.",
        insightsGroup);
    guidance->setWordWrap(true);
    guidance->setStyleSheet("color:#64748b;font-size:12px;font-weight:normal;");
    insightsLayout->addWidget(guidance);
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
    DataCache& cache = DataCache::instance();
    if (!cache.isLoaded()) {
        if (!cache.isLoading()) {
            cache.loadAsync();
        }

        if (!waitingForCacheWarmup_) {
            waitingForCacheWarmup_ = true;
            QTimer::singleShot(150, this, [this]() {
                waitingForCacheWarmup_ = false;
                loadAnalytics();
            });
        }
        return;
    }

    waitingForCacheWarmup_ = false;
    Logger::instance().info("AnalyticsWidget", "Loading analytics data...");

    cachedEngineers_ = engineerRepo_.findAll();
    cachedAssessments_ = assessmentRepo_.findAll();
    cachedAreas_ = cache.getAreas();
    cachedTotalCompetencies_ = cache.getTotalCompetencies();

    // Rebuild derived caches used by expensive analytics paths
    assessmentScoreByEngineerAndCompetency_.clear();
    competenciesByMachine_.clear();
    machinesByArea_.clear();
    machineAreaById_.clear();
    machineImportanceById_.clear();
    competencyWeightById_.clear();
    competencyMachineById_.clear();
    areaNameById_.clear();
    coreSkillAssessmentsByEngineer_.clear();
    coreSkillById_.clear();
    coreSkillCategoryNameBySkillId_.clear();
    coreSkillDisciplineBySkillId_.clear();
    engineerProductionRadarCache_.clear();
    engineerCoreSkillsRadarCache_.clear();
    cachedCoreSkills_.clear();
    cachedCoreSkillCategories_.clear();
    cachedCoreSkillAssessments_.clear();
    coreSkillsCacheLoaded_ = false;

    for (const ProductionArea& area : cachedAreas_) {
        areaNameById_[area.id()] = area.name();
        const QList<Machine> areaMachines = cache.getMachinesByArea(area.id());
        machinesByArea_[area.id()] = areaMachines;
        for (const Machine& machine : areaMachines) {
            machineAreaById_[machine.id()] = area.id();
            machineImportanceById_[machine.id()] = machine.importance();
            const QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());
            competenciesByMachine_[machine.id()] = machineCompetencies;
            for (const Competency& comp : machineCompetencies) {
                competencyWeightById_[comp.id()] = comp.calculatedWeight();
                competencyMachineById_[comp.id()] = machine.id();
            }
        }
    }

    for (const Assessment& assessment : cachedAssessments_) {
        assessmentScoreByEngineerAndCompetency_[assessment.engineerId()][assessment.competencyId()] = assessment.score();
    }

    Logger::instance().info("AnalyticsWidget",
        QString("Data loaded from cache: %1 engineers, %2 assessments, %3 competencies")
            .arg(cachedEngineers_.size())
            .arg(cachedAssessments_.size())
            .arg(cachedTotalCompetencies_));

    // Update all analytics views
    updateTrendsData();
    updateShiftComparisonData();
    insightsLoaded_ = false;
    if (insightsList_) {
        insightsList_->clear();
        QListWidgetItem* item = new QListWidgetItem(insightsList_);
        item->setText("Loading insights on demand. Open the Insights tab to generate analysis.");
        item->setBackground(QBrush(QColor("#f1f5f9")));
        item->setForeground(QBrush(QColor("#1e293b")));
        insightsList_->addItem(item);
    }

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

void AnalyticsWidget::ensureCoreSkillsCacheLoaded()
{
    if (coreSkillsCacheLoaded_) {
        return;
    }

    cachedCoreSkills_ = coreSkillsRepo_.findAllSkills();
    cachedCoreSkillCategories_ = coreSkillsRepo_.findAllCategories();
    cachedCoreSkillAssessments_ = coreSkillsRepo_.findAllAssessments();
    coreSkillAssessmentsByEngineer_.clear();
    coreSkillById_.clear();
    coreSkillCategoryNameBySkillId_.clear();
    engineerCoreSkillsRadarCache_.clear();

    QHash<QString, QString> categoryNameById;
    QHash<QString, QString> categoryDisciplineById;
    for (const CoreSkillCategory& category : cachedCoreSkillCategories_) {
        categoryNameById[category.id()] = category.name();
        categoryDisciplineById[category.id()] = category.discipline();
    }
    for (const CoreSkill& skill : cachedCoreSkills_) {
        coreSkillById_[skill.id()] = skill;
        coreSkillCategoryNameBySkillId_[skill.id()] = categoryNameById.value(skill.categoryId());
        coreSkillDisciplineBySkillId_[skill.id()] = categoryDisciplineById.value(skill.categoryId());
    }
    for (const CoreSkillAssessment& assessment : cachedCoreSkillAssessments_) {
        coreSkillAssessmentsByEngineer_[assessment.engineerId()].append(assessment);
    }

    coreSkillsCacheLoaded_ = true;
    Logger::instance().info("AnalyticsWidget",
        QString("Core skills cache loaded on demand: %1 skills, %2 assessments")
            .arg(cachedCoreSkills_.size())
            .arg(cachedCoreSkillAssessments_.size()));
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
    chart->setAnimationOptions(QChart::NoAnimation);

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
    chart->setAnimationOptions(QChart::NoAnimation);

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

    QList<Insight> insights = generateManagerInsights();

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
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability

        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);

        insightsList_->addItem(item);
    }

    if (insights.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(insightsList_);
        item->setText("👥  No Insights Available\nCollecting data to generate insights...\nAdd more assessments for automated analysis.");
        item->setBackground(QBrush(QColor("#f1f5f9")));
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

QList<AnalyticsWidget::Insight> AnalyticsWidget::generateManagerInsights()
{
    QList<Insight> result;
    QList<MachineReadiness> readiness = calculateMachineReadiness();
    std::sort(readiness.begin(), readiness.end(), [](const MachineReadiness& a, const MachineReadiness& b) {
        if (a.coveragePercent != b.coveragePercent) return a.coveragePercent < b.coveragePercent;
        return a.importance > b.importance;
    });

    int priority = 1;
    for (const MachineReadiness& machine : readiness) {
        if (result.size() >= 4 || machine.coveragePercent >= 50.0) break;
        Insight insight;
        insight.type = machine.coveragePercent < 30.0 ? "alert" : "warning";
        insight.icon = QString::number(priority++);
        insight.title = QString("Machine coverage - %1").arg(machine.machineName);
        insight.message = QString("Evidence: %1 of %2 engineers are proficient (score 2+), giving %3% coverage; %4 expert(s).\nAction: Select this machine in Engineer Development and assign the highest-impact gaps to named engineers.")
            .arg(machine.proficientCount)
            .arg(machine.totalEngineers)
            .arg(QString::number(machine.coveragePercent, 'f', 0))
            .arg(machine.expertCount);
        result.append(insight);
    }

    for (const MachineReadiness& machine : readiness) {
        if (result.size() >= 6) break;
        if (machine.expertCount <= 1 && machine.importance >= 2) {
            Insight insight;
            insight.type = "alert";
            insight.icon = QString::number(priority++);
            insight.title = QString("Resilience risk - %1").arg(machine.machineName);
            insight.message = QString("Evidence: only %1 engineer(s) are recorded at score 3 on this important machine.\nAction: develop two backups to score 2, then nominate one for train-the-trainer progression.")
                .arg(machine.expertCount);
            result.append(insight);
        }
    }

    struct EngineerNeed { QString name; int gaps = 0; int assessed = 0; };
    QList<EngineerNeed> needs;
    for (const Engineer& engineer : cachedEngineers_) {
        EngineerNeed need;
        need.name = engineer.name();
        for (const Assessment& assessment : cachedAssessments_) {
            if (assessment.engineerId() != engineer.id()) continue;
            need.assessed++;
            if (assessment.score() < 2) need.gaps++;
        }
        if (need.assessed > 0 && need.gaps > 0) needs.append(need);
    }
    std::sort(needs.begin(), needs.end(), [](const EngineerNeed& a, const EngineerNeed& b) {
        const double ar = a.assessed > 0 ? double(a.gaps) / a.assessed : 0.0;
        const double br = b.assessed > 0 ? double(b.gaps) / b.assessed : 0.0;
        return ar > br;
    });
    if (!needs.isEmpty() && result.size() < 7) {
        const EngineerNeed& need = needs.first();
        Insight insight;
        insight.type = "warning";
        insight.icon = QString::number(priority++);
        insight.title = QString("Individual development - %1").arg(need.name);
        insight.message = QString("Evidence: %1 of %2 recorded competencies are below score 2.\nAction: open Engineer Development, choose one production area, and agree a focused 90-day plan rather than assigning every gap at once.")
            .arg(need.gaps).arg(need.assessed);
        result.append(insight);
    }

    const QList<ShiftStats> shifts = calculateShiftComparison();
    if (shifts.size() > 1 && result.size() < 8) {
        const double gap = shifts.first().averageCompletion - shifts.last().averageCompletion;
        if (gap >= 10.0) {
            Insight insight;
            insight.type = "warning";
            insight.icon = QString::number(priority++);
            insight.title = "Cross-shift capability gap";
            insight.message = QString("Evidence: %1 is at %2% versus %3 at %4%, a %5-point gap.\nAction: pair competent mentors across shifts and compare the same machine scope before reallocating training time.")
                .arg(shifts.first().shiftName)
                .arg(QString::number(shifts.first().averageCompletion, 'f', 0))
                .arg(shifts.last().shiftName)
                .arg(QString::number(shifts.last().averageCompletion, 'f', 0))
                .arg(QString::number(gap, 'f', 0));
            result.append(insight);
        }
    }

    if (result.isEmpty() && !readiness.isEmpty()) {
        const auto best = std::max_element(readiness.begin(), readiness.end(), [](const MachineReadiness& a, const MachineReadiness& b) {
            return a.coveragePercent < b.coveragePercent;
        });
        Insight insight;
        insight.type = "positive";
        insight.icon = "1";
        insight.title = "Coverage is currently stable";
        insight.message = QString("Evidence: no machine is below the action threshold. %1 leads at %2% coverage.\nAction: maintain reassessment cadence and use this machine's experts as mentors.")
            .arg(best->machineName).arg(QString::number(best->coveragePercent, 'f', 0));
        result.append(insight);
    }
    return result;
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
    QList<MachineReadiness> machineReadiness = calculateMachineReadiness();
    QList<CompetencyRiskPoint> riskPoints = calculateCompetencyRisks();

    // 1. Critical Gap Alerts - from machine readiness
    for (const MachineReadiness& readiness : machineReadiness) {
        if (readiness.coveragePercent < 30) {
            Insight insight;
            insight.type = "alert";
            insight.icon = "🚨";
            insight.title = "CRITICAL GAP";
            insight.message = QString("%1 has only %2% coverage. URGENT: Train %3 engineers immediately to reach safe operating levels.")
                .arg(readiness.machineName)
                .arg(QString::number(readiness.coveragePercent, 'f', 0))
                .arg(qMax(1, int((readiness.totalEngineers * 0.7) - readiness.proficientCount)));
            insights.append(insight);
        }
    }

    // 2. Single Point of Failure
    for (const MachineReadiness& readiness : machineReadiness) {
        if (readiness.expertCount <= 1 && readiness.importance >= 2) {
            Insight insight;
            insight.type = "alert";
            insight.icon = "⚠️";
            insight.title = "SINGLE POINT OF FAILURE";
            insight.message = QString("Only %1 expert%2 can operate %3. Recommend: Cross-train 2 backup staff immediately to reduce risk.")
                .arg(readiness.expertCount)
                .arg(readiness.expertCount == 1 ? "" : "s")
                .arg(readiness.machineName);
            insights.append(insight);
        }
    }

    // 3. Training Success - identify improvements > 25%
    // (This would require historical data - simplified for now)
    if (prediction.trend == "up" && prediction.change > 25.0) {
        Insight insight;
        insight.type = "positive";
        insight.icon = "📈";
        insight.title = "TRAINING SUCCESS";
        insight.message = QString("Team competency improved %1% this period. Keep momentum: Continue current training program.")
            .arg(QString::number(prediction.change, 'f', 0));
        insights.append(insight);
    }

    // 4. Stagnation Alert - engineers with no progress
    // Check for engineers with all assessments at score 0 or 1
    QMap<QString, bool> engineerHasProgress;
    for (const Engineer& engineer : cachedEngineers_) {
        bool hasGoodScore = false;
        for (const Assessment& assessment : cachedAssessments_) {
            if (assessment.engineerId() == engineer.id() && assessment.score() >= 2) {
                hasGoodScore = true;
                break;
            }
        }
        if (!hasGoodScore && !cachedAssessments_.isEmpty()) {
            engineerHasProgress[engineer.name()] = false;
        }
    }

    if (engineerHasProgress.size() >= 3) {
        Insight insight;
        insight.type = "warning";
        insight.icon = "⏸️";
        insight.title = "LEARNING PLATEAU";
        insight.message = QString("%1 engineers show no proficient competencies. Action: Review training approach and provide additional support.")
            .arg(engineerHasProgress.size());
        insights.append(insight);
    }

    // 5. Cross-Training Opportunity - shifts with complementary strengths
    if (shifts.size() >= 2) {
        // Find shifts with significant performance gap
        for (int i = 0; i < shifts.size() - 1; i++) {
            for (int j = i + 1; j < shifts.size(); j++) {
                double gap = qAbs(shifts[i].averageCompletion - shifts[j].averageCompletion);
                if (gap > 15.0) {
                    Insight insight;
                    insight.type = "positive";
                    insight.icon = "🔄";
                    insight.title = "CROSS-TRAINING OPPORTUNITY";
                    insight.message = QString("%1 (%2% proficiency) and %3 (%4% proficiency) have different strengths. Swap 2 engineers for skill diversity.")
                        .arg(shifts[i].shiftName)
                        .arg(QString::number(shifts[i].averageCompletion, 'f', 0))
                        .arg(shifts[j].shiftName)
                        .arg(QString::number(shifts[j].averageCompletion, 'f', 0));
                    insights.append(insight);
                    break;  // Only suggest once
                }
            }
        }
    }

    // 6. Promotion Ready - engineers with > 90% proficiency
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

    for (const EngineerScore& score : engineerScores) {
        if (score.percentage >= 90.0) {
            Insight insight;
            insight.type = "positive";
            insight.icon = "👤";
            insight.title = "PROMOTION READY";
            insight.message = QString("%1: %2% proficiency achieved. Consider: Lead operator role or mentorship responsibilities.")
                .arg(score.name)
                .arg(QString::number(score.percentage, 'f', 0));
            insights.append(insight);
            break;  // Only show top candidate
        }
    }

    // 7. Unbalanced Skills - areas with high variance
    QMap<QString, double> areaProficiency;
    QMap<QString, int> areaCount;

    for (const ProductionArea& area : cachedAreas_) {
        const QList<Machine>& areaMachines = machinesByArea_[area.id()];
        for (const Machine& machine : areaMachines) {
            const QList<Competency>& machineCompetencies = competenciesByMachine_[machine.id()];
            for (const Competency& comp : machineCompetencies) {
                int totalScore = 0;
                int count = 0;
                for (const Assessment& assessment : cachedAssessments_) {
                    if (assessment.competencyId() == comp.id()) {
                        totalScore += assessment.score();
                        count++;
                    }
                }
                if (count > 0) {
                    double proficiency = (double(totalScore) / count / 3.0) * 100.0;
                    areaProficiency[area.name()] += proficiency;
                    areaCount[area.name()]++;
                }
            }
        }
    }

    // Find areas with significant imbalance
    QList<QPair<QString, double>> areaAverages;
    for (auto it = areaProficiency.begin(); it != areaProficiency.end(); ++it) {
        if (areaCount[it.key()] > 0) {
            areaAverages.append(qMakePair(it.key(), it.value() / areaCount[it.key()]));
        }
    }

    if (areaAverages.size() >= 2) {
        std::sort(areaAverages.begin(), areaAverages.end(),
                  [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                      return a.second > b.second;
                  });

        double gap = areaAverages.first().second - areaAverages.last().second;
        if (gap > 30.0) {
            Insight insight;
            insight.type = "warning";
            insight.icon = "⚖️";
            insight.title = "UNBALANCED SKILLS";
            insight.message = QString("%1 area: %2% proficiency vs %3 area: %4% proficiency. Focus: Balance training resources.")
                .arg(areaAverages.first().first)
                .arg(QString::number(areaAverages.first().second, 'f', 0))
                .arg(areaAverages.last().first)
                .arg(QString::number(areaAverages.last().second, 'f', 0));
            insights.append(insight);
        }
    }

    // 8. Resource Allocation - calculate training needs per area
    QMap<QString, int> areaTrainingNeeds;
    for (const CompetencyRiskPoint& point : riskPoints) {
        if (point.riskLevel == "critical") {
            const int machineId = competencyMachineById_.value(point.competencyId, -1);
            const int areaId = machineAreaById_.value(machineId, -1);
            const QString areaName = areaNameById_.value(areaId);
            if (!areaName.isEmpty()) {
                areaTrainingNeeds[areaName]++;
            }
        }
    }

    if (!areaTrainingNeeds.isEmpty()) {
        // Find area with most training needs
        QString maxArea;
        int maxNeeds = 0;
        for (auto it = areaTrainingNeeds.begin(); it != areaTrainingNeeds.end(); ++it) {
            if (it.value() > maxNeeds) {
                maxNeeds = it.value();
                maxArea = it.key();
            }
        }

        if (maxNeeds >= 3) {
            Insight insight;
            insight.type = "warning";
            insight.icon = "💰";
            insight.title = "RESOURCE ALLOCATION";
            insight.message = QString("%1 area needs %2x more training hours than other areas. Budget: Allocate additional training resources.")
                .arg(maxArea)
                .arg(maxNeeds);
            insights.append(insight);
        }
    }

    // 9. High Impact Training - top ROI opportunities
    QList<TrainingRecommendation> recommendations = calculateTrainingROI();
    if (!recommendations.isEmpty() && recommendations.first().priority == "urgent") {
        Insight insight;
        insight.type = "alert";
        insight.icon = "🎯";
        insight.title = "HIGH IMPACT OPPORTUNITY";
        insight.message = QString("Training %1 engineers in '%2' would improve coverage by 25%% (Impact: %3/10)")
            .arg(recommendations.first().engineersNeed)
            .arg(recommendations.first().competencyName)
            .arg(QString::number(recommendations.first().impactScore, 'f', 1));
        insights.append(insight);
    }

    // 10. Shift Vulnerability - shifts with most critical gaps
    QMap<QString, int> shiftCriticalMachines;
    for (const MachineReadiness& readiness : machineReadiness) {
        if (readiness.coveragePercent < 50) {
            for (const Engineer& engineer : cachedEngineers_) {
                QString shift = engineer.shift();
                if (!shift.isEmpty()) {
                    shiftCriticalMachines[shift]++;
                }
            }
        }
    }

    if (!shiftCriticalMachines.isEmpty()) {
        QString maxShift;
        int maxCount = 0;
        for (auto it = shiftCriticalMachines.begin(); it != shiftCriticalMachines.end(); ++it) {
            if (it.value() > maxCount) {
                maxCount = it.value();
                maxShift = it.key();
            }
        }

        if (maxCount >= 3) {
            Insight insight;
            insight.type = "alert";
            insight.icon = "📊";
            insight.title = "SHIFT VULNERABILITY";
            insight.message = QString("%1: %2 machines under 50%% coverage. Risk: Production disruption if absences occur.")
                .arg(maxShift)
                .arg(maxCount);
            insights.append(insight);
        }
    }

    // Existing insights (trend and top performer) - keep for continuity
    if (prediction.trend == "up" && prediction.change <= 25.0) {
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
    machineReadinessButton_->setStyleSheet(tabIndex == 3 ? activeStyle : inactiveStyle);
    engineerRadarButton_->setStyleSheet(tabIndex == 4 ? activeStyle : inactiveStyle);
    shiftOverviewButton_->setStyleSheet(tabIndex == 5 ? activeStyle : inactiveStyle);

    // Update data for newly selected tab
    if (tabIndex == 2) {
        if (!insightsLoaded_) {
            QTimer::singleShot(0, this, [this]() {
                updateAutomatedInsights();
                insightsLoaded_ = true;
            });
        }
    } else if (tabIndex == 3) {
        QTimer::singleShot(0, this, [this]() { updateMachineReadinessData(); });
    } else if (tabIndex == 4) {
        QTimer::singleShot(0, this, [this]() { engineerDevelopmentWidget_->refresh(); });
    } else if (tabIndex == 5) {
        QTimer::singleShot(0, this, [this]() { updateShiftOverviewData(); });
    }
}

void AnalyticsWidget::onRefreshClicked()
{
    loadAnalytics();
    if (engineerDevelopmentWidget_) {
        engineerDevelopmentWidget_->refresh();
    }
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

    // Set palette for text colors (must be set before stylesheet on macOS)
    QPalette engineerPalette;
    engineerPalette.setColor(QPalette::WindowText, Qt::black);
    engineerPalette.setColor(QPalette::Text, Qt::black);
    engineerPalette.setColor(QPalette::ButtonText, Qt::black);
    engineerPalette.setColor(QPalette::Base, Qt::white);
    engineerPalette.setColor(QPalette::Window, Qt::white);
    engineerPalette.setColor(QPalette::Button, Qt::white);
    engineerPalette.setColor(QPalette::Highlight, QColor(59, 130, 246));  // #3b82f6
    engineerPalette.setColor(QPalette::HighlightedText, Qt::white);
    engineerSelector_->setPalette(engineerPalette);

    engineerSelector_->setStyleSheet(
        "QComboBox {"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    background-color: white;"
        "    color: #1e293b;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #3b82f6;"
        "    selection-color: white;"
        "}"
    );
    engineerSelector_->setMaxVisibleItems(15);
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
    zoomOutBtn->setFixedSize(28, 28);
    zoomOutBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
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
    zoomInBtn->setFixedSize(28, 28);
    zoomInBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
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
        "Core Skills by Discipline",
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

    // Set palette for text colors (must be set before stylesheet on macOS)
    QPalette shiftFilterPalette;
    shiftFilterPalette.setColor(QPalette::WindowText, Qt::black);
    shiftFilterPalette.setColor(QPalette::Text, Qt::black);
    shiftFilterPalette.setColor(QPalette::ButtonText, Qt::black);
    shiftFilterPalette.setColor(QPalette::Base, Qt::white);
    shiftFilterPalette.setColor(QPalette::Window, Qt::white);
    shiftFilterPalette.setColor(QPalette::Button, Qt::white);
    shiftFilterPalette.setColor(QPalette::Highlight, QColor(59, 130, 246));  // #3b82f6
    shiftFilterPalette.setColor(QPalette::HighlightedText, Qt::white);
    shiftFilterCombo_->setPalette(shiftFilterPalette);

    shiftFilterCombo_->setStyleSheet(
        "QComboBox {"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    background-color: white;"
        "    color: #1e293b;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #3b82f6;"
        "    selection-color: white;"
        "}"
    );
    shiftFilterCombo_->setMaxVisibleItems(10);
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

    // Set palette for text colors (must be set before stylesheet on macOS)
    QPalette dataTypePalette;
    dataTypePalette.setColor(QPalette::WindowText, Qt::black);
    dataTypePalette.setColor(QPalette::Text, Qt::black);
    dataTypePalette.setColor(QPalette::ButtonText, Qt::black);
    dataTypePalette.setColor(QPalette::Base, Qt::white);
    dataTypePalette.setColor(QPalette::Window, Qt::white);
    dataTypePalette.setColor(QPalette::Button, Qt::white);
    dataTypePalette.setColor(QPalette::Highlight, QColor(59, 130, 246));  // #3b82f6
    dataTypePalette.setColor(QPalette::HighlightedText, Qt::white);
    shiftDataTypeCombo_->setPalette(dataTypePalette);

    shiftDataTypeCombo_->setStyleSheet(
        "QComboBox {"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    background-color: white;"
        "    color: #1e293b;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    color: #1e293b;"
        "    selection-background-color: #3b82f6;"
        "    selection-color: white;"
        "}"
    );
    shiftDataTypeCombo_->setMaxVisibleItems(5);
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
    zoomOutBtn->setFixedSize(28, 28);
    zoomOutBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
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
    zoomInBtn->setFixedSize(28, 28);
    zoomInBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    border: 1px solid #cbd5e1;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
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
    chart->setAnimationOptions(QPolarChart::NoAnimation);

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
    chart->setAnimationOptions(QPolarChart::NoAnimation);

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
    const auto cachedResultIt = engineerProductionRadarCache_.constFind(engineerId);
    if (cachedResultIt != engineerProductionRadarCache_.constEnd()) {
        return cachedResultIt.value();
    }

    QMap<QString, double> radarData;

    const auto engineerScoresIt = assessmentScoreByEngineerAndCompetency_.constFind(engineerId);
    if (engineerScoresIt == assessmentScoreByEngineerAndCompetency_.constEnd() || engineerScoresIt->isEmpty()) {
        engineerProductionRadarCache_.insert(engineerId, radarData);
        return radarData;
    }

    QMap<QString, double> areaWeightedSum;
    QMap<QString, double> areaTotalWeights;

    for (auto it = engineerScoresIt->constBegin(); it != engineerScoresIt->constEnd(); ++it) {
        const int competencyId = it.key();
        const int score = it.value();

        const int machineId = competencyMachineById_.value(competencyId, -1);
        if (machineId <= 0) {
            continue;
        }

        const int areaId = machineAreaById_.value(machineId, -1);
        const QString areaName = areaNameById_.value(areaId);
        if (areaName.isEmpty()) {
            continue;
        }

        const double competencyWeight = competencyWeightById_.value(competencyId, 0.0);
        const double machineImpact = static_cast<double>(machineImportanceById_.value(machineId, 0));
        const double combinedWeight = competencyWeight * (machineImpact + 1.0);
        if (combinedWeight <= 0.0) {
            continue;
        }

        areaWeightedSum[areaName] += score * combinedWeight;
        areaTotalWeights[areaName] += combinedWeight;
    }

    for (auto it = areaWeightedSum.constBegin(); it != areaWeightedSum.constEnd(); ++it) {
        const QString areaName = it.key();
        if (areaTotalWeights[areaName] > 0) {
            radarData[areaName] = it.value() / areaTotalWeights[areaName];
        }
    }

    engineerProductionRadarCache_.insert(engineerId, radarData);
    return radarData;
}

QMap<QString, double> AnalyticsWidget::calculateEngineerCoreSkillsRadarData(const QString& engineerId)
{
    ensureCoreSkillsCacheLoaded();

    const auto cachedResultIt = engineerCoreSkillsRadarCache_.constFind(engineerId);
    if (cachedResultIt != engineerCoreSkillsRadarCache_.constEnd()) {
        return cachedResultIt.value();
    }

    QMap<QString, double> radarData;

    const QList<CoreSkillAssessment> engineerAssessments = coreSkillAssessmentsByEngineer_.value(engineerId);
    if (engineerAssessments.isEmpty()) {
        engineerCoreSkillsRadarCache_.insert(engineerId, radarData);
        return radarData;
    }

    QMap<QString, double> disciplineWeightedSum = {
        {"Mechanical", 0.0},
        {"Electrical", 0.0},
        {"Software", 0.0}
    };
    QMap<QString, double> disciplineTotalWeights = {
        {"Mechanical", 0.0},
        {"Electrical", 0.0},
        {"Software", 0.0}
    };

    for (const CoreSkillAssessment& assessment : engineerAssessments) {
        const auto skillIt = coreSkillById_.constFind(assessment.skillId());
        if (skillIt == coreSkillById_.constEnd()) {
            continue;
        }

        const QString categoryName = coreSkillCategoryNameBySkillId_.value(assessment.skillId());
        const QString discipline = mapCoreSkillDiscipline(
            skillIt->categoryId(), categoryName, skillIt->name(),
            coreSkillDisciplineBySkillId_.value(skillIt->id()));
        if (discipline == "Unknown") {
            continue;
        }

        const double weight = skillIt->calculatedWeight();
        disciplineWeightedSum[discipline] += assessment.score() * weight;
        disciplineTotalWeights[discipline] += weight;
    }

    const QStringList disciplines = {"Mechanical", "Electrical", "Software"};
    for (const QString& discipline : disciplines) {
        if (disciplineTotalWeights[discipline] > 0) {
            radarData[discipline] = disciplineWeightedSum[discipline] / disciplineTotalWeights[discipline];
        } else {
            radarData[discipline] = 0.0;
        }
    }

    engineerCoreSkillsRadarCache_.insert(engineerId, radarData);
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

// ============================================================================
// CRITICAL SKILLS DASHBOARD TAB
// ============================================================================

void AnalyticsWidget::setupCriticalSkillsTab(QWidget* criticalSkillsWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(criticalSkillsWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    // Risk Matrix Scatter Plot
    QGroupBox* riskMatrixGroup = new QGroupBox("Competency Risk Matrix", this);
    riskMatrixGroup->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* riskMatrixLayout = new QVBoxLayout(riskMatrixGroup);
    riskMatrixChartView_ = new QChartView(this);
    riskMatrixChartView_->setRenderHint(QPainter::Antialiasing);
    riskMatrixChartView_->setMinimumHeight(400);
    riskMatrixLayout->addWidget(riskMatrixChartView_);

    layout->addWidget(riskMatrixGroup);

    // Training Priority List
    QGroupBox* priorityGroup = new QGroupBox("Top Training Priorities (Ranked by ROI)", this);
    priorityGroup->setStyleSheet(
        "QGroupBox {"
        "    background-color: white;"
        "    border: 2px solid #e2e8f0;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "}"
    );

    QVBoxLayout* priorityLayout = new QVBoxLayout(priorityGroup);
    trainingPriorityList_ = new QListWidget(this);
    trainingPriorityList_->setSpacing(12);
    trainingPriorityList_->setStyleSheet(
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
    priorityLayout->addWidget(trainingPriorityList_);

    layout->addWidget(priorityGroup);
}

void AnalyticsWidget::updateCriticalSkillsData()
{
    // Calculate risk points for all competencies
    QList<CompetencyRiskPoint> riskPoints = calculateCompetencyRisks();

    // Create scatter plot for risk matrix
    QChart* chart = new QChart();
    chart->setTitle("");
    chart->setAnimationOptions(QChart::NoAnimation);

    // Create series for each quadrant
    QScatterSeries* criticalSeries = new QScatterSeries();
    criticalSeries->setName("Critical (Train Now)");
    criticalSeries->setColor(QColor("#ff6b6b"));  // Red
    criticalSeries->setMarkerSize(12);

    QScatterSeries* maintainSeries = new QScatterSeries();
    maintainSeries->setName("Maintain");
    maintainSeries->setColor(QColor("#4ade80"));  // Green
    maintainSeries->setMarkerSize(12);

    QScatterSeries* deferSeries = new QScatterSeries();
    deferSeries->setName("Defer");
    deferSeries->setColor(QColor("#94a3b8"));  // Gray
    deferSeries->setMarkerSize(12);

    QScatterSeries* excessSeries = new QScatterSeries();
    excessSeries->setName("Excess Capacity");
    excessSeries->setColor(QColor("#60a5fa"));  // Blue
    excessSeries->setMarkerSize(12);

    // Add points to appropriate series
    for (const CompetencyRiskPoint& point : riskPoints) {
        if (point.riskLevel == "critical") {
            criticalSeries->append(point.proficiency, point.importance);
        } else if (point.riskLevel == "maintain") {
            maintainSeries->append(point.proficiency, point.importance);
        } else if (point.riskLevel == "defer") {
            deferSeries->append(point.proficiency, point.importance);
        } else if (point.riskLevel == "excess") {
            excessSeries->append(point.proficiency, point.importance);
        }
    }

    chart->addSeries(criticalSeries);
    chart->addSeries(maintainSeries);
    chart->addSeries(deferSeries);
    chart->addSeries(excessSeries);

    // X-axis: Proficiency (0-100%)
    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("Team Proficiency %");
    axisX->setRange(0, 100);
    axisX->setTickCount(11);
    chart->addAxis(axisX, Qt::AlignBottom);
    criticalSeries->attachAxis(axisX);
    maintainSeries->attachAxis(axisX);
    deferSeries->attachAxis(axisX);
    excessSeries->attachAxis(axisX);

    // Y-axis: Importance (0-5.0)
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Business Importance");
    axisY->setRange(0, 5);
    axisY->setTickCount(6);
    chart->addAxis(axisY, Qt::AlignLeft);
    criticalSeries->attachAxis(axisY);
    maintainSeries->attachAxis(axisY);
    deferSeries->attachAxis(axisY);
    excessSeries->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    riskMatrixChartView_->setChart(chart);

    // Update training priority list
    trainingPriorityList_->clear();
    QList<TrainingRecommendation> recommendations = calculateTrainingROI();

    for (const TrainingRecommendation& rec : recommendations) {
        QListWidgetItem* item = new QListWidgetItem(trainingPriorityList_);

        QString backgroundColor, icon;
        if (rec.priority == "urgent") {
            backgroundColor = "#fee2e2";  // Light red
            icon = "⚠️";
        } else if (rec.priority == "high") {
            backgroundColor = "#fef3c7";  // Light yellow
            icon = "🎯";
        } else {
            backgroundColor = "#e0f2fe";  // Light blue
            icon = "💡";
        }

        QString itemText = QString("%1 %2: %3\n    Coverage: %4 need training | Impact: %5/10\n    %6")
            .arg(icon)
            .arg(rec.priority.toUpper())
            .arg(rec.competencyName)
            .arg(rec.engineersNeed)
            .arg(QString::number(rec.impactScore, 'f', 1))
            .arg(rec.reason);

        item->setText(itemText);
        item->setBackground(QBrush(QColor(backgroundColor)));
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability

        QFont itemFont = item->font();
        itemFont.setPointSize(13);
        item->setFont(itemFont);

        trainingPriorityList_->addItem(item);
    }

    if (recommendations.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(trainingPriorityList_);
        item->setText("✅ No urgent training priorities\nAll competencies are well-staffed!");
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);
    }

    Logger::instance().info("AnalyticsWidget", QString("Updated Critical Skills data: %1 risk points, %2 recommendations").arg(riskPoints.size()).arg(recommendations.size()));
}

// ============================================================================
// MACHINE READINESS ANALYSIS TAB
// ============================================================================

void AnalyticsWidget::setupMachineReadinessTab(QWidget* machineReadinessWidget)
{
    QVBoxLayout* layout = new QVBoxLayout(machineReadinessWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Production area", machineReadinessWidget);
    filterLabel->setStyleSheet("color:#475569;font-weight:600;");
    machineReadinessAreaFilter_ = new QComboBox(machineReadinessWidget);
    machineReadinessAreaFilter_->setMinimumWidth(240);
    machineReadinessAreaFilter_->addItem("All production areas", 0);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(machineReadinessAreaFilter_);
    filterLayout->addStretch();
    layout->addLayout(filterLayout);

    QGroupBox* chartGroup = new QGroupBox("Top 5 Machines to Focus On", machineReadinessWidget);
    chartGroup->setStyleSheet(
        "QGroupBox{background:white;border:1px solid #dbe4ef;border-radius:10px;"
        "padding:16px;font-size:14pt;font-weight:700;}");
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    QLabel* chartHelp = new QLabel(
        "Select a production area to rank its five lowest machine knowledge scores. Missing assessments are shown separately from the average.",
        chartGroup);
    chartHelp->setWordWrap(true);
    chartHelp->setStyleSheet("color:#64748b;font-size:11px;font-weight:normal;");
    chartLayout->addWidget(chartHelp);
    machineReadinessList_ = new QListWidget(chartGroup);
    machineReadinessList_->setSpacing(10);
    machineReadinessList_->setFixedHeight(540);
    machineReadinessList_->setSelectionMode(QAbstractItemView::NoSelection);
    machineReadinessList_->setFocusPolicy(Qt::NoFocus);
    machineReadinessList_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    machineReadinessList_->setStyleSheet(
        "QListWidget {"
        "    border: none;"
        "    background-color: transparent;"
        "}"
        "QListWidget::item { border:none; padding:0; margin:0; }"
    );
    chartLayout->addWidget(machineReadinessList_);
    layout->addWidget(chartGroup);

    // Retained off-screen for the existing risk calculation path; the simplified page
    // deliberately presents only the top-five focus list.
    vulnerabilityList_ = new QListWidget(this);
    vulnerabilityList_->hide();

    connect(machineReadinessAreaFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { updateMachineReadinessData(); });
}

void AnalyticsWidget::updateMachineReadinessData()
{
    machineReadinessList_->clear();
    vulnerabilityList_->clear();

    QList<MachineReadiness> readinessList = calculateMachineReadiness();

    if (machineReadinessAreaFilter_->count() <= 1) {
        machineReadinessAreaFilter_->blockSignals(true);
        for (const ProductionArea& area : cachedAreas_) {
            machineReadinessAreaFilter_->addItem(area.name(), area.id());
        }
        machineReadinessAreaFilter_->blockSignals(false);
    }

    const int areaFilter = machineReadinessAreaFilter_->currentData().toInt();
    if (areaFilter > 0) {
        readinessList.erase(std::remove_if(readinessList.begin(), readinessList.end(),
            [areaFilter](const MachineReadiness& readiness) { return readiness.areaId != areaFilter; }),
            readinessList.end());
    }

    // Lowest knowledge first gives managers an immediate training-priority reading path.
    std::sort(readinessList.begin(), readinessList.end(),
              [](const MachineReadiness& a, const MachineReadiness& b) {
                  if (a.averageScore != b.averageScore) return a.averageScore < b.averageScore;
                  return a.coveragePercent < b.coveragePercent;
              });

    // Render five compact, self-contained focus cards.
    int displayedMachines = 0;
    for (const MachineReadiness& readiness : readinessList) {
        if (displayedMachines >= 5) break;
        QListWidgetItem* item = new QListWidgetItem(machineReadinessList_);

        QString backgroundColor, accentColor, statusText, actionText;
        if (readiness.averageScore < 1.0 || readiness.coveragePercent < 30) {
            backgroundColor = "#fff7f7";
            accentColor = "#ef4444";
            statusText = "PRIORITY";
            actionText = "Assign immediate training and nominate two backups.";
        } else if (readiness.averageScore < 1.5 || readiness.coveragePercent < 50) {
            backgroundColor = "#fffbeb";
            accentColor = "#f59e0b";
            statusText = "DEVELOPING";
            actionText = "Increase supervised occurrences for the lowest scorers.";
        } else if (readiness.averageScore < 2.0 || readiness.coveragePercent < 70) {
            backgroundColor = "#f0f7ff";
            accentColor = "#2563eb";
            statusText = "PROGRESSING";
            actionText = "Close remaining gaps and validate practical evidence.";
        } else {
            backgroundColor = "#f0fdf4";
            accentColor = "#10b981";
            statusText = "READY";
            actionText = "Maintain competence and develop another trainer.";
        }

        QWidget* card = new QWidget(machineReadinessList_);
        card->setObjectName("machineFocusCard");
        card->setStyleSheet(QString(
            "QWidget#machineFocusCard{background:%1;border:1px solid #dbe4ef;"
            "border-left:5px solid %2;border-radius:10px;}").arg(backgroundColor, accentColor));
        QHBoxLayout* cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(16, 10, 16, 10);
        cardLayout->setSpacing(16);

        QLabel* rank = new QLabel(QString::number(displayedMachines + 1), card);
        rank->setFixedSize(38, 38);
        rank->setAlignment(Qt::AlignCenter);
        rank->setStyleSheet(QString(
            "background:%1;color:white;border-radius:19px;font-size:16px;font-weight:800;").arg(accentColor));
        cardLayout->addWidget(rank);

        QVBoxLayout* identityLayout = new QVBoxLayout();
        QLabel* machineName = new QLabel(readiness.machineName, card);
        machineName->setStyleSheet("color:#0f172a;font-size:15px;font-weight:700;");
        QLabel* context = new QLabel(QString("%1  |  %2").arg(readiness.areaName, statusText), card);
        context->setStyleSheet(QString("color:%1;font-size:11px;font-weight:700;").arg(accentColor));
        identityLayout->addWidget(machineName);
        identityLayout->addWidget(context);
        cardLayout->addLayout(identityLayout, 2);

        QVBoxLayout* scoreLayout = new QVBoxLayout();
        QLabel* scoreHeading = new QLabel("AVERAGE SCORE", card);
        scoreHeading->setStyleSheet("color:#64748b;font-size:10px;font-weight:700;");
        QProgressBar* scoreBar = new QProgressBar(card);
        scoreBar->setRange(0, 300);
        scoreBar->setValue(qRound(readiness.averageScore * 100.0));
        scoreBar->setFormat(QString("%1 / 3").arg(readiness.averageScore, 0, 'f', 2));
        scoreBar->setMinimumWidth(250);
        scoreBar->setFixedHeight(24);
        scoreBar->setStyleSheet(QString(
            "QProgressBar{background:#e2e8f0;border:0;border-radius:6px;text-align:center;"
            "color:#0f172a;font-weight:700;}QProgressBar::chunk{background:%1;border-radius:6px;}").arg(accentColor));
        scoreLayout->addWidget(scoreHeading);
        scoreLayout->addWidget(scoreBar);
        cardLayout->addLayout(scoreLayout, 2);

        QVBoxLayout* evidenceLayout = new QVBoxLayout();
        QLabel* completion = new QLabel(QString("%1% assessed").arg(
            QString::number(readiness.assessmentCoveragePercent, 'f', 0)), card);
        QLabel* proficiency = new QLabel(QString("%1/%2 trained  |  %3 experts")
            .arg(readiness.proficientCount).arg(readiness.totalEngineers).arg(readiness.expertCount), card);
        completion->setStyleSheet("color:#334155;font-weight:600;");
        proficiency->setStyleSheet("color:#64748b;font-size:11px;");
        evidenceLayout->addWidget(completion);
        evidenceLayout->addWidget(proficiency);
        cardLayout->addLayout(evidenceLayout, 2);

        QLabel* action = new QLabel(actionText, card);
        action->setWordWrap(true);
        action->setMinimumWidth(230);
        action->setStyleSheet("color:#334155;font-size:11px;");
        cardLayout->addWidget(action, 2);

        item->setSizeHint(QSize(0, 92));
        item->setData(Qt::UserRole, readiness.machineId);
        machineReadinessList_->setItemWidget(item, card);
        displayedMachines++;
    }

    // Generate vulnerability alerts
    for (const MachineReadiness& readiness : readinessList) {
        // Single point of failure (only 1 expert)
        if (readiness.expertCount <= 1 && readiness.importance >= 2) {
            QListWidgetItem* item = new QListWidgetItem(vulnerabilityList_);
            item->setText(QString("⚠️ SINGLE POINT OF FAILURE\n    %1: Only %2 expert%3\n    Action: Cross-train 2 backup engineers immediately")
                .arg(readiness.machineName)
                .arg(readiness.expertCount)
                .arg(readiness.expertCount == 1 ? "" : "s"));
            item->setBackground(QBrush(QColor("#fee2e2")));
            item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
            QFont itemFont = item->font();
            itemFont.setPointSize(13);
            item->setFont(itemFont);
            vulnerabilityList_->addItem(item);
        }

        // Critical gap (< 30% coverage)
        if (readiness.coveragePercent < 30) {
            QListWidgetItem* item = new QListWidgetItem(vulnerabilityList_);
            item->setText(QString("🚨 CRITICAL GAP\n    %1: %2% coverage\n    Action: URGENT - Train %3 engineers to reach 70% coverage")
                .arg(readiness.machineName)
                .arg(QString::number(readiness.coveragePercent, 'f', 0))
                .arg(qMax(1, int((readiness.totalEngineers * 0.7) - readiness.proficientCount))));
            item->setBackground(QBrush(QColor("#fee2e2")));
            item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
            QFont itemFont = item->font();
            itemFont.setPointSize(13);
            item->setFont(itemFont);
            vulnerabilityList_->addItem(item);
        }
    }

    // Shift vulnerability analysis
    QMap<QString, int> shiftVulnerabilities;

    QHash<QString, QSet<int>> proficientMachinesByShift;
    for (const Engineer& engineer : cachedEngineers_) {
        const QString shift = engineer.shift();
        if (shift.isEmpty()) {
            continue;
        }
        const auto engineerScoresIt = assessmentScoreByEngineerAndCompetency_.constFind(engineer.id());
        if (engineerScoresIt == assessmentScoreByEngineerAndCompetency_.constEnd()) {
            continue;
        }

        for (auto scoreIt = engineerScoresIt->constBegin(); scoreIt != engineerScoresIt->constEnd(); ++scoreIt) {
            if (scoreIt.value() < 2) {
                continue;
            }
            const int machineId = competencyMachineById_.value(scoreIt.key(), -1);
            if (machineId > 0) {
                proficientMachinesByShift[shift].insert(machineId);
            }
        }
    }

    for (const MachineReadiness& readiness : readinessList) {
        if (readiness.coveragePercent < 50) {
            for (auto it = proficientMachinesByShift.constBegin(); it != proficientMachinesByShift.constEnd(); ++it) {
                if (!it.value().contains(readiness.machineId)) {
                    shiftVulnerabilities[it.key()]++;
                }
            }
        }
    }

    // Display shift vulnerability summary
    if (!shiftVulnerabilities.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(vulnerabilityList_);
        QString shiftText = "📊 SHIFT VULNERABILITY\n";
        for (auto it = shiftVulnerabilities.begin(); it != shiftVulnerabilities.end(); ++it) {
            shiftText += QString("    %1: %2 vulnerable machines\n").arg(it.key()).arg(it.value());
        }
        shiftText += "    Risk: Production disruption if absences occur";
        item->setText(shiftText);
        item->setBackground(QBrush(QColor("#fef3c7")));
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
        QFont itemFont = item->font();
        itemFont.setPointSize(13);
        item->setFont(itemFont);
        vulnerabilityList_->addItem(item);
    }

    if (vulnerabilityList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(vulnerabilityList_);
        item->setText("✅ No Critical Vulnerabilities\nAll machines have adequate coverage!");
        item->setBackground(QBrush(QColor("#d1fae5")));
        item->setForeground(QBrush(QColor("#1e293b")));  // Dark text for readability
        QFont itemFont = item->font();
        itemFont.setPointSize(14);
        item->setFont(itemFont);
        vulnerabilityList_->addItem(item);
    }

    Logger::instance().info("AnalyticsWidget", QString("Updated Machine Readiness data: %1 machines").arg(readinessList.size()));
}

// ============================================================================
// CALCULATION METHODS
// ============================================================================

QList<AnalyticsWidget::CompetencyRiskPoint> AnalyticsWidget::calculateCompetencyRisks()
{
    QList<CompetencyRiskPoint> riskPoints;

    QList<Competency> allCompetencies;

    for (const ProductionArea& area : cachedAreas_) {
        const QList<Machine>& areaMachines = machinesByArea_[area.id()];
        for (const Machine& machine : areaMachines) {
            allCompetencies.append(competenciesByMachine_[machine.id()]);
        }
    }

    QHash<int, int> totalScoreByCompetency;
    QHash<int, int> countByCompetency;
    for (const Assessment& assessment : cachedAssessments_) {
        totalScoreByCompetency[assessment.competencyId()] += assessment.score();
        countByCompetency[assessment.competencyId()] += 1;
    }

    for (const Competency& comp : allCompetencies) {
        CompetencyRiskPoint point;
        point.name = comp.name();
        point.competencyId = comp.id();
        point.importance = comp.calculatedWeight();

        const int totalScore = totalScoreByCompetency.value(comp.id(), 0);
        const int assessmentCount = countByCompetency.value(comp.id(), 0);
        if (assessmentCount > 0) {
            double avgScore = double(totalScore) / assessmentCount;
            point.proficiency = (avgScore / 3.0) * 100.0;
        } else {
            point.proficiency = 0.0;
        }

        if (point.importance >= 2.5 && point.proficiency < 50) {
            point.riskLevel = "critical";
        } else if (point.importance >= 2.5 && point.proficiency >= 50) {
            point.riskLevel = "maintain";
        } else if (point.importance < 2.5 && point.proficiency < 50) {
            point.riskLevel = "defer";
        } else {
            point.riskLevel = "excess";
        }

        riskPoints.append(point);
    }

    return riskPoints;
}

QList<AnalyticsWidget::MachineReadiness> AnalyticsWidget::calculateMachineReadiness()
{
    QList<MachineReadiness> readinessList;

    for (const ProductionArea& area : cachedAreas_) {
        const QList<Machine>& areaMachines = machinesByArea_[area.id()];

        for (const Machine& machine : areaMachines) {
            MachineReadiness readiness;
            readiness.areaName = area.name();
            readiness.areaId = area.id();
            readiness.machineName = machine.name();
            readiness.machineId = machine.id();
            readiness.importance = machine.importance();
            readiness.totalEngineers = cachedEngineers_.size();
            readiness.proficientCount = 0;
            readiness.expertCount = 0;
            readiness.averageScore = 0.0;
            readiness.assessmentCoveragePercent = 0.0;

            const QList<Competency>& machineCompetencies = competenciesByMachine_[machine.id()];
            readiness.competencyCount = machineCompetencies.size();

            if (machineCompetencies.isEmpty()) {
                readiness.coveragePercent = 0;
                readiness.isCritical = true;
                readinessList.append(readiness);
                continue;
            }

            double weightedScoreTotal = 0.0;
            double recordedWeightTotal = 0.0;
            int recordedAssessmentCount = 0;
            for (const Engineer& engineer : cachedEngineers_) {
                const auto engineerScoresIt = assessmentScoreByEngineerAndCompetency_.constFind(engineer.id());
                int totalScore = 0;
                int maxScore = 0;

                for (const Competency& comp : machineCompetencies) {
                    if (engineerScoresIt != assessmentScoreByEngineerAndCompetency_.constEnd()) {
                        const auto scoreIt = engineerScoresIt->constFind(comp.id());
                        if (scoreIt != engineerScoresIt->constEnd()) {
                            totalScore += scoreIt.value();
                            maxScore += 3;
                            const double weight = competencyWeightById_.value(comp.id(), comp.calculatedWeight());
                            weightedScoreTotal += scoreIt.value() * weight;
                            recordedWeightTotal += weight;
                            recordedAssessmentCount++;
                        }
                    }
                }

                if (maxScore > 0) {
                    const double avgScore = static_cast<double>(totalScore) / (maxScore / 3);

                    if (avgScore >= 2.0) {
                        readiness.proficientCount++;
                    }
                    if (avgScore >= 2.5) {
                        readiness.expertCount++;
                    }
                }
            }

            if (recordedWeightTotal > 0.0) {
                readiness.averageScore = weightedScoreTotal / recordedWeightTotal;
            }
            const int possibleAssessmentCount = readiness.totalEngineers * readiness.competencyCount;
            if (possibleAssessmentCount > 0) {
                readiness.assessmentCoveragePercent =
                    (double(recordedAssessmentCount) / possibleAssessmentCount) * 100.0;
            }

            // Calculate coverage percentage
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

QList<AnalyticsWidget::TrainingRecommendation> AnalyticsWidget::calculateTrainingROI()
{
    QList<TrainingRecommendation> recommendations;

    // Get competency risks
    QList<CompetencyRiskPoint> riskPoints = calculateCompetencyRisks();

    // Filter to critical competencies only
    for (const CompetencyRiskPoint& point : riskPoints) {
        if (point.riskLevel == "critical") {
            TrainingRecommendation rec;
            rec.competencyName = point.name;

            // Count how many engineers need training (score < 2)
            rec.engineersNeed = 0;
            for (const Assessment& assessment : cachedAssessments_) {
                if (assessment.competencyId() == point.competencyId && assessment.score() < 2) {
                    rec.engineersNeed++;
                }
            }

            // Calculate impact score (0-10 scale)
            // Formula: importance (0-5) + (engineersNeed / totalEngineers) * 5
            double engineerFactor = cachedEngineers_.isEmpty() ? 0 : (double(rec.engineersNeed) / cachedEngineers_.size()) * 5.0;
            rec.impactScore = point.importance + engineerFactor;
            rec.impactScore = qMin(rec.impactScore, 10.0);

            // Determine priority
            if (rec.impactScore >= 7.0) {
                rec.priority = "urgent";
            } else if (rec.impactScore >= 5.0) {
                rec.priority = "high";
            } else {
                rec.priority = "medium";
            }

            rec.reason = QString("High business importance (%1/5.0) with low team proficiency (%2%)")
                .arg(QString::number(point.importance, 'f', 1))
                .arg(QString::number(point.proficiency, 'f', 0));

            recommendations.append(rec);
        }
    }

    // Sort by impact score (descending)
    std::sort(recommendations.begin(), recommendations.end(),
              [](const TrainingRecommendation& a, const TrainingRecommendation& b) {
                  return a.impactScore > b.impactScore;
              });

    // Limit to top 10 recommendations
    if (recommendations.size() > 10) {
        recommendations = recommendations.mid(0, 10);
    }

    return recommendations;
}
