#include "MyDashboardWidget.h"
#include "../utils/Logger.h"
#include "../core/DataCache.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QScatterSeries>
#include <QtMath>
#include <QSet>
#include <QShowEvent>
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

QString coreSkillDiscipline(const QString& categoryId, const QString& categoryName,
                            const QString& skillName = QString(), const QString& storedDiscipline = QString())
{
    if (!storedDiscipline.trimmed().isEmpty()) {
        return storedDiscipline.trimmed();
    }
    const QString id = categoryId.toLower();
    const QString name = categoryName.toLower();
    const QString skill = skillName.toLower();
    const QString combined = QString("%1 %2 %3").arg(id, name, skill);

    const QStringList mechanicalTokens = {
        "mechanical", "mech", "lathe", "mill", "pump", "pneumatic", "hydraulic",
        "gearbox", "bearing", "shaft", "belt", "chain", "alignment"
    };
    const QStringList electricalTokens = {
        "electrical", "elec", "motor", "control panel", "wiring", "relay", "contactor",
        "voltage", "current", "inverter", "vfd", "drive", "sensor"
    };
    const QStringList softwareTokens = {
        "software", "plc", "hmi", "scada", "logic", "program", "automation",
        "rockwell", "siemens", "tia", "studio 5000", "gx developer", "code"
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

QString disciplineButtonStyle(bool selected)
{
    if (selected) {
        return
            "QPushButton {"
            "  padding: 8px 14px;"
            "  min-height: 40px;"
            "  border-radius: 10px;"
            "  border: 2px solid #b45309;"
            "  background-color: #f59e0b;"
            "  color: #ffffff;"
            "  font-size: 14px;"
            "  font-weight: 700;"
            "}"
            "QPushButton:hover {"
            "  border-color: #92400e;"
            "  background-color: #d97706;"
            "  color: #ffffff;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #b45309;"
            "}";
    }

    return
        "QPushButton {"
        "  padding: 8px 14px;"
        "  min-height: 40px;"
        "  border-radius: 10px;"
        "  border: 2px solid #94a3b8;"
        "  background-color: #ffffff;"
        "  color: #0f172a;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  border-color: #f59e0b;"
        "  background-color: #fff7ed;"
        "  color: #92400e;"
        "}"
        "QPushButton:pressed {"
        "  border-color: #d97706;"
        "  background-color: #ffedd5;"
        "  color: #7c2d12;"
        "}";
}
}

MyDashboardWidget::MyDashboardWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , isFirstShow_(true)
    , totalAssessmentsLabel_(nullptr)
    , avgSkillLevelLabel_(nullptr)
    , coreSkillsAvgLabel_(nullptr)
    , competenciesAssessedLabel_(nullptr)
    , weaknessListWidget_(nullptr)
    , strengthsListWidget_(nullptr)
    , productionAreaChartView_(nullptr)
    , coreSkillsChartView_(nullptr)
    , coreSkillDrilldownLabel_(nullptr)
    , coreSkillGapListWidget_(nullptr)
    , coreSkillStrengthListWidget_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
}

MyDashboardWidget::~MyDashboardWidget()
{
}

void MyDashboardWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (isFirstShow_) {
        isFirstShow_ = false;
        loadDashboardData();
    }
}

void MyDashboardWidget::setupUI()
{
    // Main layout for the widget
    QVBoxLayout* widgetLayout = new QVBoxLayout(this);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(0);

    // Create scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    // Create container widget for scroll content
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Title and refresh button
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("My Dashboard", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    refreshButton_ = new QPushButton("Refresh", this);
    connect(refreshButton_, &QPushButton::clicked, this, &MyDashboardWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton_);

    mainLayout->addLayout(headerLayout);

    // Personal stats section - 4 stat cards in a row
    QGroupBox* statsGroup = new QGroupBox("Performance Snapshot", this);
    QHBoxLayout* statsLayout = new QHBoxLayout(statsGroup);
    statsLayout->setSpacing(15);

    // Stat card 1: Total Assessments
    QGroupBox* stat1 = new QGroupBox(this);
    QVBoxLayout* stat1Layout = new QVBoxLayout(stat1);
    QLabel* stat1Title = new QLabel("Total Assessments", this);
    stat1Title->setStyleSheet("font-weight: bold; color: #666;");
    totalAssessmentsLabel_ = new QLabel("0", this);
    QFont statFont;
    statFont.setPointSize(24);
    statFont.setBold(true);
    totalAssessmentsLabel_->setFont(statFont);
    totalAssessmentsLabel_->setStyleSheet("color: #2196F3;");
    stat1Layout->addWidget(stat1Title);
    stat1Layout->addWidget(totalAssessmentsLabel_);
    stat1Layout->addStretch();
    statsLayout->addWidget(stat1);

    // Stat card 2: Average Skill Level
    QGroupBox* stat2 = new QGroupBox(this);
    QVBoxLayout* stat2Layout = new QVBoxLayout(stat2);
    QLabel* stat2Title = new QLabel("Avg Competency Level", this);
    stat2Title->setStyleSheet("font-weight: bold; color: #666;");
    avgSkillLevelLabel_ = new QLabel("0.0", this);
    avgSkillLevelLabel_->setFont(statFont);
    avgSkillLevelLabel_->setStyleSheet("color: #4CAF50;");
    stat2Layout->addWidget(stat2Title);
    stat2Layout->addWidget(avgSkillLevelLabel_);
    stat2Layout->addStretch();
    statsLayout->addWidget(stat2);

    // Stat card 3: Core Skills Average
    QGroupBox* stat3 = new QGroupBox(this);
    QVBoxLayout* stat3Layout = new QVBoxLayout(stat3);
    QLabel* stat3Title = new QLabel("Avg Core Skills", this);
    stat3Title->setStyleSheet("font-weight: bold; color: #666;");
    coreSkillsAvgLabel_ = new QLabel("0.0", this);
    coreSkillsAvgLabel_->setFont(statFont);
    coreSkillsAvgLabel_->setStyleSheet("color: #FF9800;");
    stat3Layout->addWidget(stat3Title);
    stat3Layout->addWidget(coreSkillsAvgLabel_);
    stat3Layout->addStretch();
    statsLayout->addWidget(stat3);

    // Stat card 4: Readiness
    QGroupBox* stat4 = new QGroupBox(this);
    QVBoxLayout* stat4Layout = new QVBoxLayout(stat4);
    QLabel* stat4Title = new QLabel("Readiness (2+)", this);
    stat4Title->setStyleSheet("font-weight: bold; color: #666;");
    competenciesAssessedLabel_ = new QLabel("0", this);
    competenciesAssessedLabel_->setFont(statFont);
    competenciesAssessedLabel_->setStyleSheet("color: #9C27B0;");
    stat4Layout->addWidget(stat4Title);
    stat4Layout->addWidget(competenciesAssessedLabel_);
    stat4Layout->addStretch();
    statsLayout->addWidget(stat4);

    mainLayout->addWidget(statsGroup);

    // Radar charts section - two charts side by side
    QGroupBox* chartsGroup = new QGroupBox("Skills Overview", this);
    QHBoxLayout* chartsLayout = new QHBoxLayout(chartsGroup);
    chartsLayout->setSpacing(15);

    // Production Area Radar Chart
    QVBoxLayout* prodChartLayout = new QVBoxLayout();
    QLabel* prodChartTitle = new QLabel("Production Area Scores", this);
    prodChartTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    prodChartTitle->setAlignment(Qt::AlignCenter);
    productionAreaChartView_ = new QChartView(this);
    productionAreaChartView_->setRenderHint(QPainter::Antialiasing);
    productionAreaChartView_->setMinimumHeight(500);
    productionAreaChartView_->setMinimumWidth(500);
    productionAreaChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    prodChartLayout->addWidget(prodChartTitle);
    prodChartLayout->addWidget(productionAreaChartView_, 1);
    chartsLayout->addLayout(prodChartLayout, 1);

    // Core Skills Radar Chart
    QVBoxLayout* coreChartLayout = new QVBoxLayout();
    QLabel* coreChartTitle = new QLabel("Core Skills Scores", this);
    coreChartTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    coreChartTitle->setAlignment(Qt::AlignCenter);
    coreSkillsChartView_ = new QChartView(this);
    coreSkillsChartView_->setRenderHint(QPainter::Antialiasing);
    coreSkillsChartView_->setMinimumHeight(500);
    coreSkillsChartView_->setMinimumWidth(500);
    coreSkillsChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    coreChartLayout->addWidget(coreChartTitle);
    coreChartLayout->addWidget(coreSkillsChartView_, 1);
    chartsLayout->addLayout(coreChartLayout, 1);

    mainLayout->addWidget(chartsGroup);

    // Core skills discipline drill-down
    QGroupBox* drilldownGroup = new QGroupBox("Core Skills Discipline Drill-Down", this);
    QVBoxLayout* drilldownLayout = new QVBoxLayout(drilldownGroup);
    coreSkillDrilldownLabel_ = new QLabel("Click Mechanical, Electrical, or Software on the radar to inspect details.", this);
    coreSkillDrilldownLabel_->setStyleSheet("color: #475569; font-style: italic;");
    drilldownLayout->addWidget(coreSkillDrilldownLabel_);

    QHBoxLayout* disciplineButtonsLayout = new QHBoxLayout();
    const QStringList disciplineFilters = {"Mechanical", "Electrical", "Software"};
    for (const QString& discipline : disciplineFilters) {
        QPushButton* disciplineButton = new QPushButton(discipline, this);
        disciplineButton->setCursor(Qt::PointingHandCursor);
        disciplineButton->setStyleSheet(disciplineButtonStyle(false));
        connect(disciplineButton, &QPushButton::clicked, this, [this, discipline]() {
            updateCoreSkillDisciplineDrilldown(discipline);
        });
        disciplineButtons_[discipline] = disciplineButton;
        disciplineButtonsLayout->addWidget(disciplineButton);
    }
    disciplineButtonsLayout->addStretch();
    drilldownLayout->addLayout(disciplineButtonsLayout);

    QHBoxLayout* drilldownListsLayout = new QHBoxLayout();
    QGroupBox* gapsGroup = new QGroupBox("Top Gaps", this);
    QVBoxLayout* gapsLayout = new QVBoxLayout(gapsGroup);
    coreSkillGapListWidget_ = new QListWidget(this);
    coreSkillGapListWidget_->setMinimumHeight(150);
    gapsLayout->addWidget(coreSkillGapListWidget_);
    drilldownListsLayout->addWidget(gapsGroup, 1);

    QGroupBox* drilldownStrengthsGroup = new QGroupBox("Top Strengths", this);
    QVBoxLayout* drilldownStrengthsLayout = new QVBoxLayout(drilldownStrengthsGroup);
    coreSkillStrengthListWidget_ = new QListWidget(this);
    coreSkillStrengthListWidget_->setMinimumHeight(150);
    drilldownStrengthsLayout->addWidget(coreSkillStrengthListWidget_);
    drilldownListsLayout->addWidget(drilldownStrengthsGroup, 1);

    drilldownLayout->addLayout(drilldownListsLayout);
    mainLayout->addWidget(drilldownGroup);

    // Development and strengths section
    QHBoxLayout* developmentLayout = new QHBoxLayout();
    developmentLayout->setSpacing(15);

    QGroupBox* weaknessGroup = new QGroupBox("Priority Development Areas", this);
    QVBoxLayout* weaknessLayout = new QVBoxLayout(weaknessGroup);
    QLabel* weaknessDesc = new QLabel("Focus first on lowest-scoring items for fastest impact:", this);
    weaknessDesc->setStyleSheet("color: #666; font-style: italic;");
    weaknessLayout->addWidget(weaknessDesc);
    weaknessListWidget_ = new QListWidget(this);
    weaknessListWidget_->setMaximumHeight(190);
    weaknessLayout->addWidget(weaknessListWidget_);
    developmentLayout->addWidget(weaknessGroup, 1);

    QGroupBox* strengthsGroup = new QGroupBox("Top Strengths", this);
    QVBoxLayout* strengthsLayout = new QVBoxLayout(strengthsGroup);
    QLabel* strengthsDesc = new QLabel("Strongest skills where you can support peers:", this);
    strengthsDesc->setStyleSheet("color: #666; font-style: italic;");
    strengthsLayout->addWidget(strengthsDesc);
    strengthsListWidget_ = new QListWidget(this);
    strengthsListWidget_->setMaximumHeight(190);
    strengthsLayout->addWidget(strengthsListWidget_);
    developmentLayout->addWidget(strengthsGroup, 1);

    mainLayout->addLayout(developmentLayout);
    mainLayout->addStretch();

    // Wire up scroll area
    scrollArea->setWidget(scrollContent);
    widgetLayout->addWidget(scrollArea);
}

void MyDashboardWidget::loadDashboardData()
{
    Logger::instance().info("MyDashboardWidget", QString("Loading dashboard for engineer: %1").arg(engineerId_));

    // Load engineer data
    currentEngineer_ = engineerRepo_.findById(engineerId_);

    // Load only this engineer's data to keep dashboard fast at scale.
    assessments_ = assessmentRepo_.findByEngineer(engineerId_);
    coreSkillAssessments_ = coreSkillsRepo_.findAssessmentsByEngineer(engineerId_);

    // Update all sections
    updatePersonalStats();
    updateAreasOfWeakness();
    createProductionAreaRadarChart();
    createCoreSkillsRadarChart();
    updateCoreSkillDisciplineDrilldown("Mechanical");
}

void MyDashboardWidget::updatePersonalStats()
{
    // Total assessments (competencies + core skills)
    int totalAssessments = assessments_.size() + coreSkillAssessments_.size();
    totalAssessmentsLabel_->setText(QString::number(totalAssessments));

    // Average competency skill level (WEIGHTED)
    double avgCompetency = 0.0;
    if (!assessments_.isEmpty()) {
        // Load all competencies to get weights
        DataCache& cache = DataCache::instance();
        QList<ProductionArea> areas = cache.getAreas();
        QList<Competency> allCompetencies;
        for (const ProductionArea& area : areas) {
            QList<Machine> machines = cache.getMachinesByArea(area.id());
            for (const Machine& machine : machines) {
                QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());
                allCompetencies.append(machineCompetencies);
            }
        }

        double weightedSum = 0.0;
        double totalWeights = 0.0;

        for (const Assessment& assessment : assessments_) {
            // Find the competency to get its weight
            for (const Competency& comp : allCompetencies) {
                if (comp.id() == assessment.competencyId()) {
                    double weight = comp.calculatedWeight();
                    weightedSum += assessment.score() * weight;
                    totalWeights += weight;
                    break;
                }
            }
        }

        if (totalWeights > 0.0) {
            avgCompetency = weightedSum / totalWeights;
        }
    }
    avgSkillLevelLabel_->setText(QString::number(avgCompetency, 'f', 2));

    // Average core skills (WEIGHTED)
    double avgCoreSkills = 0.0;
    if (!coreSkillAssessments_.isEmpty()) {
        QList<CoreSkill> allCoreSkills = coreSkillsRepo_.findAllSkills();

        double weightedSum = 0.0;
        double totalWeights = 0.0;

        for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
            // Find the core skill to get its weight
            for (const CoreSkill& skill : allCoreSkills) {
                if (skill.id() == assessment.skillId()) {
                    double weight = skill.calculatedWeight();
                    weightedSum += assessment.score() * weight;
                    totalWeights += weight;
                    break;
                }
            }
        }

        if (totalWeights > 0.0) {
            avgCoreSkills = weightedSum / totalWeights;
        }
    }
    coreSkillsAvgLabel_->setText(QString::number(avgCoreSkills, 'f', 2));

    // Readiness percentage (share of all assessments at level 2+).
    int readyCount = 0;
    for (const Assessment& assessment : assessments_) {
        if (assessment.score() >= 2) {
            readyCount++;
        }
    }
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        if (assessment.score() >= 2) {
            readyCount++;
        }
    }
    const int totalItems = assessments_.size() + coreSkillAssessments_.size();
    const double readinessPercent = (totalItems > 0)
        ? (static_cast<double>(readyCount) * 100.0 / static_cast<double>(totalItems))
        : 0.0;
    competenciesAssessedLabel_->setText(QString("%1%").arg(QString::number(readinessPercent, 'f', 0)));
}

void MyDashboardWidget::updateAreasOfWeakness()
{
    weaknessListWidget_->clear();
    strengthsListWidget_->clear();

    // Build lookup data
    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    DataCache& cache = DataCache::instance();
    QMap<int, QString> areaNamesById;
    QMap<int, QString> machineNamesById;
    QMap<int, QString> competencyNamesById;

    for (const ProductionArea& area : areas) {
        areaNamesById.insert(area.id(), area.name());
        QList<Machine> areaMachines = cache.getMachinesByArea(area.id());
        for (const Machine& machine : areaMachines) {
            machineNamesById.insert(machine.id(), machine.name());
            QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());
            for (const Competency& competency : machineCompetencies) {
                competencyNamesById.insert(competency.id(), competency.name());
            }
        }
    }

    QList<QString> weaknessItems;
    QList<QPair<int, QString>> strengthItems;

    QList<CoreSkill> allCoreSkills = coreSkillsRepo_.findAllSkills();
    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QMap<QString, QString> categoryNamesById;
    QMap<QString, QString> coreSkillNamesById;
    for (const CoreSkillCategory& category : categories) {
        categoryNamesById.insert(category.id(), category.name());
    }
    for (const CoreSkill& skill : allCoreSkills) {
        coreSkillNamesById.insert(skill.id(), skill.name());
    }

    // Production competencies
    for (const Assessment& assessment : assessments_) {
        const QString areaName = areaNamesById.value(assessment.productionAreaId(), "Area");
        const QString machineName = machineNamesById.value(assessment.machineId(), "Machine");
        const QString competencyName = competencyNamesById.value(assessment.competencyId(), "Competency");
        const QString descriptor = QString("%1 | %2 | %3").arg(areaName).arg(machineName).arg(competencyName);

        if (assessment.score() <= 1) {
            weaknessItems.append(QString("%1 (Score %2) - target next: reach 2")
                .arg(descriptor).arg(assessment.score()));
        } else if (assessment.score() >= 3) {
            strengthItems.append(qMakePair(assessment.score(), QString("%1 (Score %2)")
                .arg(descriptor).arg(assessment.score())));
        }
    }

    // Core skills
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        const QString categoryName = categoryNamesById.value(assessment.categoryId(), "Category");
        const QString skillName = coreSkillNamesById.value(assessment.skillId(), "Skill");
        const QString descriptor = QString("Core Skill | %1 | %2").arg(categoryName).arg(skillName);

        if (assessment.score() <= 1) {
            weaknessItems.append(QString("%1 (Score %2) - target next: reach 2")
                .arg(descriptor).arg(assessment.score()));
        } else if (assessment.score() >= 3) {
            strengthItems.append(qMakePair(assessment.score(), QString("%1 (Score %2)")
                .arg(descriptor).arg(assessment.score())));
        }
    }

    std::sort(strengthItems.begin(), strengthItems.end(), [](const QPair<int, QString>& a, const QPair<int, QString>& b) {
        return a.first > b.first;
    });

    for (const QString& item : weaknessItems) {
        weaknessListWidget_->addItem(item);
    }
    const int maxStrengthItems = std::min(5, static_cast<int>(strengthItems.size()));
    for (int i = 0; i < maxStrengthItems; ++i) {
        strengthsListWidget_->addItem(strengthItems[i].second);
    }

    if (weaknessListWidget_->count() == 0) {
        weaknessListWidget_->addItem("No urgent development gaps. Maintain current standards.");
    }
    if (strengthsListWidget_->count() == 0) {
        strengthsListWidget_->addItem("No score-3 strengths yet. Keep building toward expert level.");
    }
}

void MyDashboardWidget::createProductionAreaRadarChart()
{
    // Calculate WEIGHTED average score per production area
    QMap<QString, double> areaWeightedScores;
    QMap<QString, double> areaTotalWeights;

    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Initialize maps
    for (const ProductionArea& area : areas) {
        areaWeightedScores[area.name()] = 0.0;
        areaTotalWeights[area.name()] = 0.0;
    }

    // Load all competencies from cache
    DataCache& cache = DataCache::instance();
    QList<Competency> allCompetencies;
    for (const ProductionArea& area : areas) {
        QList<Machine> machines = cache.getMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> machineCompetencies = cache.getCompetenciesByMachine(machine.id());
            allCompetencies.append(machineCompetencies);
        }
    }

    // Calculate weighted sums
    for (const Assessment& assessment : assessments_) {
        int areaId = assessment.productionAreaId();

        // Find the competency to get its weight
        double weight = 3.0; // Default weight if not found
        for (const Competency& comp : allCompetencies) {
            if (comp.id() == assessment.competencyId()) {
                weight = comp.calculatedWeight();
                break;
            }
        }

        for (const ProductionArea& area : areas) {
            if (area.id() == areaId) {
                areaWeightedScores[area.name()] += assessment.score() * weight;
                areaTotalWeights[area.name()] += weight;
                break;
            }
        }
    }

    // Compute weighted averages
    QMap<QString, double> averages;
    for (const QString& areaName : areaWeightedScores.keys()) {
        if (areaTotalWeights[areaName] > 0.0) {
            averages[areaName] = areaWeightedScores[areaName] / areaTotalWeights[areaName];
        } else {
            averages[areaName] = 0.0;
        }
    }

    QChart* chart = createRadarChart(averages, "Production Areas (Weighted)", QColor("#2196F3"));
    productionAreaChartView_->setChart(chart);
}

void MyDashboardWidget::createCoreSkillsRadarChart()
{
    // Calculate WEIGHTED average score grouped by discipline (3 points only).
    QMap<QString, double> disciplineWeightedScores{
        {"Mechanical", 0.0},
        {"Electrical", 0.0},
        {"Software", 0.0}
    };
    QMap<QString, double> disciplineTotalWeights{
        {"Mechanical", 0.0},
        {"Electrical", 0.0},
        {"Software", 0.0}
    };

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> allCoreSkills = coreSkillsRepo_.findAllSkills();

    QMap<QString, QString> categoryNameById;
    QMap<QString, QString> categoryDisciplineById;
    for (const CoreSkillCategory& category : categories) {
        categoryNameById.insert(category.id(), category.name());
        categoryDisciplineById.insert(category.id(), category.discipline());
    }

    QMap<QString, CoreSkill> coreSkillById;
    for (const CoreSkill& skill : allCoreSkills) {
        coreSkillById.insert(skill.id(), skill);
    }

    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        const QString categoryId = assessment.categoryId();
        const QString categoryName = categoryNameById.value(categoryId);
        const QString skillName = coreSkillById.contains(assessment.skillId())
            ? coreSkillById.value(assessment.skillId()).name()
            : QString();
        const QString discipline = coreSkillDiscipline(
            categoryId, categoryName, skillName, categoryDisciplineById.value(categoryId));
        if (discipline == "Unknown") {
            continue;
        }

        double weight = 3.0; // Default weight if not found
        const auto skillIt = coreSkillById.constFind(assessment.skillId());
        if (skillIt != coreSkillById.cend()) {
            weight = skillIt.value().calculatedWeight();
        }

        disciplineWeightedScores[discipline] += assessment.score() * weight;
        disciplineTotalWeights[discipline] += weight;
    }

    const QStringList disciplines = {"Mechanical", "Electrical", "Software"};
    coreSkillRadarLabels_ = disciplines;

    QPolarChart* chart = new QPolarChart();
    chart->setTitle("Core Skills by Discipline (Weighted)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QLineSeries* radarLine = new QLineSeries();
    radarLine->setName("Score");
    QScatterSeries* radarPoints = new QScatterSeries();
    radarPoints->setName("Click to drill down");
    radarPoints->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    radarPoints->setMarkerSize(22.0);
    radarPoints->setColor(QColor("#FF9800"));
    radarPoints->setBorderColor(QColor("#B45309"));

    for (int i = 0; i < disciplines.size(); ++i) {
        const QString& discipline = disciplines[i];
        double value = 0.0;
        if (disciplineTotalWeights[discipline] > 0.0) {
            value = disciplineWeightedScores[discipline] / disciplineTotalWeights[discipline];
        }
        radarLine->append(i, value);
        radarPoints->append(i, value);
    }
    if (!disciplines.isEmpty()) {
        const double firstValue = radarLine->points().isEmpty() ? 0.0 : radarLine->points().first().y();
        radarLine->append(disciplines.size(), firstValue);
    }

    QLineSeries* lowerSeries = new QLineSeries();
    for (int i = 0; i <= disciplines.size(); ++i) {
        lowerSeries->append(i, 0);
    }

    QAreaSeries* areaSeries = new QAreaSeries(radarLine, lowerSeries);
    areaSeries->setName("Score");
    QColor fillColor("#FF9800");
    fillColor.setAlpha(100);
    areaSeries->setColor(fillColor);
    areaSeries->setBorderColor(QColor("#FF9800"));

    chart->addSeries(areaSeries);
    chart->addSeries(radarPoints);

    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < disciplines.size(); ++i) {
        angularAxis->append(disciplines[i], i);
    }
    angularAxis->setRange(0, disciplines.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setTickCount(4);
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    areaSeries->attachAxis(angularAxis);
    areaSeries->attachAxis(radialAxis);
    radarPoints->attachAxis(angularAxis);
    radarPoints->attachAxis(radialAxis);

    chart->legend()->hide();
    coreSkillsChartView_->setChart(chart);

    connect(radarPoints, &QScatterSeries::clicked, this, &MyDashboardWidget::onCoreSkillRadarPointClicked);
}

void MyDashboardWidget::updateCoreSkillDisciplineDrilldown(const QString& discipline)
{
    if (!coreSkillGapListWidget_ || !coreSkillStrengthListWidget_ || !coreSkillDrilldownLabel_) {
        return;
    }

    activeDiscipline_ = discipline;
    updateDisciplineButtonStyles();

    coreSkillGapListWidget_->clear();
    coreSkillStrengthListWidget_->clear();

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();

    QMap<QString, QString> categoryNameById;
    QMap<QString, QString> categoryDisciplineById;
    QSet<QString> disciplineCategoryIds;
    for (const CoreSkillCategory& category : categories) {
        categoryNameById[category.id()] = category.name();
        categoryDisciplineById[category.id()] = category.discipline();
        if (coreSkillDiscipline(category.id(), category.name(), QString(), category.discipline()) == discipline) {
            disciplineCategoryIds.insert(category.id());
        }
    }

    QMap<QString, CoreSkill> skillById;
    for (const CoreSkill& skill : skills) {
        skillById[skill.id()] = skill;
    }

    struct DrilldownItem {
        int score = 0;
        double weight = 0.0;
        QString text;
    };
    QList<DrilldownItem> priorityTrainingItems;
    QList<DrilldownItem> maintainStrengthItems;

    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        if (!disciplineCategoryIds.contains(assessment.categoryId())) {
            continue;
        }

        const CoreSkill skill = skillById.value(assessment.skillId());
        const QString mappedDiscipline = coreSkillDiscipline(
            assessment.categoryId(), categoryNameById.value(assessment.categoryId()), skill.name(),
            categoryDisciplineById.value(assessment.categoryId()));
        if (mappedDiscipline != discipline) {
            continue;
        }
        const QString categoryName = categoryNameById.value(assessment.categoryId(), "Category");
        const QString skillName = skill.id().isEmpty() ? "Skill" : skill.name();
        const int maxScore = skill.id().isEmpty() ? 3 : skill.maxScore();
        const double weight = skill.id().isEmpty() ? 1.0 : skill.calculatedWeight();
        const QString line = QString("%1 | %2: %3/%4  (weight %.1f)")
            .arg(categoryName)
            .arg(skillName)
            .arg(assessment.score())
            .arg(maxScore)
            .arg(weight, 0, 'f', 1);

        DrilldownItem item;
        item.score = assessment.score();
        item.weight = weight;
        item.text = line;

        if (assessment.score() <= 1) {
            priorityTrainingItems.append(item);
        } else if (assessment.score() >= 3) {
            maintainStrengthItems.append(item);
        }
    }

    // Priorities: lowest score first, then highest impact (weight) first.
    std::sort(priorityTrainingItems.begin(), priorityTrainingItems.end(), [](const DrilldownItem& a, const DrilldownItem& b) {
        if (a.score != b.score) {
            return a.score < b.score;
        }
        return a.weight > b.weight;
    });

    // Strengths: expert items with highest impact (weight) first.
    std::sort(maintainStrengthItems.begin(), maintainStrengthItems.end(), [](const DrilldownItem& a, const DrilldownItem& b) {
        if (a.weight != b.weight) {
            return a.weight > b.weight;
        }
        return a.score > b.score;
    });

    const int maxItems = 5;
    for (int i = 0; i < std::min(maxItems, static_cast<int>(priorityTrainingItems.size())); ++i) {
        const DrilldownItem& item = priorityTrainingItems[i];
        QString tag = (item.score == 0) ? "[Urgent]" : "[Build]";
        QString label = QString("%1 %2").arg(tag, item.text);
        QListWidgetItem* listItem = new QListWidgetItem(label, coreSkillGapListWidget_);
        if (item.score == 0) {
            listItem->setForeground(QBrush(QColor("#B91C1C")));
        } else {
            listItem->setForeground(QBrush(QColor("#92400E")));
        }
    }
    for (int i = 0; i < std::min(maxItems, static_cast<int>(maintainStrengthItems.size())); ++i) {
        const DrilldownItem& item = maintainStrengthItems[i];
        QString label = QString("[Expert] %1").arg(item.text);
        QListWidgetItem* listItem = new QListWidgetItem(label, coreSkillStrengthListWidget_);
        listItem->setForeground(QBrush(QColor("#166534")));
    }

    if (coreSkillGapListWidget_->count() == 0) {
        coreSkillGapListWidget_->addItem("No score 0/1 items in this discipline. Good baseline coverage.");
    }
    if (coreSkillStrengthListWidget_->count() == 0) {
        coreSkillStrengthListWidget_->addItem("No score 3 items yet in this discipline.");
    }

    coreSkillDrilldownLabel_->setText(QString("Showing %1 detail for %2").arg(currentEngineer_.name(), discipline));
}

void MyDashboardWidget::updateDisciplineButtonStyles()
{
    for (auto it = disciplineButtons_.begin(); it != disciplineButtons_.end(); ++it) {
        const bool isSelected = (it.key() == activeDiscipline_);
        if (it.value()) {
            it.value()->setStyleSheet(disciplineButtonStyle(isSelected));
        }
    }
}

void MyDashboardWidget::onCoreSkillRadarPointClicked(const QPointF& point)
{
    if (coreSkillRadarLabels_.isEmpty()) {
        return;
    }

    int index = qRound(point.x());
    if (index < 0 || index >= coreSkillRadarLabels_.size()) {
        return;
    }
    updateCoreSkillDisciplineDrilldown(coreSkillRadarLabels_.at(index));
}

QChart* MyDashboardWidget::createRadarChart(const QMap<QString, double>& data, const QString& title, const QColor& color)
{
    QPolarChart* chart = new QPolarChart();
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Create line series for radar
    QLineSeries* series = new QLineSeries();
    series->setName("Score");

    // Add data points (need to close the polygon by adding first point at end)
    int index = 0;
    QStringList labels;
    for (auto it = data.begin(); it != data.end(); ++it) {
        series->append(index, it.value());
        labels.append(it.key());
        index++;
    }

    // Close the radar by adding first point at the end
    if (!data.isEmpty()) {
        series->append(index, data.first());
    }

    // Create area series for filled radar
    QLineSeries* lowerSeries = new QLineSeries();
    for (int i = 0; i <= index; i++) {
        lowerSeries->append(i, 0);
    }

    QAreaSeries* areaSeries = new QAreaSeries(series, lowerSeries);
    areaSeries->setName("Score");
    QColor fillColor = color;
    fillColor.setAlpha(100);
    areaSeries->setColor(fillColor);
    areaSeries->setBorderColor(color);

    chart->addSeries(areaSeries);

    // Configure angular axis (categories)
    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < labels.size(); i++) {
        angularAxis->append(labels[i], i);
    }
    angularAxis->setRange(0, labels.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    // Configure radial axis (scores 0-3)
    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setTickCount(4);
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    areaSeries->attachAxis(angularAxis);
    areaSeries->attachAxis(radialAxis);

    chart->legend()->hide();

    return chart;
}

void MyDashboardWidget::onRefreshClicked()
{
    loadDashboardData();
}
