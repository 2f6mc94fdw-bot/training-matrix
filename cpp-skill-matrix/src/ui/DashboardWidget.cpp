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
#include <QTimer>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QScatterSeries>
#include <QLinearGradient>
#include <QSharedPointer>
#include <QSet>
#include <QtConcurrent/QtConcurrent>

#include <algorithm>

namespace {
using MachineReadiness = DashboardWidget::MachineReadiness;
using TrainingRecommendation = DashboardWidget::TrainingRecommendation;
using ShiftPerformance = DashboardWidget::ShiftPerformance;
using EngineerScoreRowData = DashboardWidget::EngineerScoreRowData;

QList<MachineReadiness> computeMachineReadiness(
    const QList<Engineer>& engineers,
    const QList<ProductionArea>& areas,
    const QHash<int, QList<Machine>>& machinesByArea,
    const QHash<int, QList<Competency>>& competenciesByMachine,
    const QHash<QString, QHash<int, int>>& assessmentScores)
{
    QList<MachineReadiness> readinessList;

    for (const ProductionArea& area : areas) {
        const QList<Machine>& areaMachines = machinesByArea.value(area.id());
        for (const Machine& machine : areaMachines) {
            MachineReadiness readiness;
            readiness.machineName = machine.name();
            readiness.machineId = machine.id();
            readiness.importance = machine.importance();
            readiness.totalEngineers = engineers.size();
            readiness.proficientCount = 0;
            readiness.expertCount = 0;

            const QList<Competency>& machineCompetencies = competenciesByMachine.value(machine.id());
            if (machineCompetencies.isEmpty()) {
                readiness.coveragePercent = 0;
                readiness.isCritical = true;
                readinessList.append(readiness);
                continue;
            }

            for (const Engineer& engineer : engineers) {
                int totalScore = 0;
                int maxScore = 0;
                const auto engineerScoresIt = assessmentScores.constFind(engineer.id());
                if (engineerScoresIt != assessmentScores.constEnd()) {
                    for (const Competency& comp : machineCompetencies) {
                        const auto scoreIt = engineerScoresIt->constFind(comp.id());
                        if (scoreIt != engineerScoresIt->constEnd()) {
                            totalScore += scoreIt.value();
                            maxScore += 3;
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

            readiness.coveragePercent = readiness.totalEngineers > 0
                ? (static_cast<double>(readiness.proficientCount) / readiness.totalEngineers) * 100.0
                : 0.0;
            readiness.isCritical = readiness.coveragePercent < 50.0;
            readinessList.append(readiness);
        }
    }

    return readinessList;
}

QList<TrainingRecommendation> computeTrainingPriorities(
    const QList<Engineer>& engineers,
    const QList<ProductionArea>& areas,
    const QList<Assessment>& assessments,
    const QHash<int, QList<Machine>>& machinesByArea,
    const QHash<int, QList<Competency>>& competenciesByMachine)
{
    QList<TrainingRecommendation> recommendations;
    QList<Competency> allCompetencies;
    for (const ProductionArea& area : areas) {
        const QList<Machine>& areaMachines = machinesByArea.value(area.id());
        for (const Machine& machine : areaMachines) {
            allCompetencies.append(competenciesByMachine.value(machine.id()));
        }
    }

    QHash<int, int> totalScoreByCompetency;
    QHash<int, int> assessmentCountByCompetency;
    QHash<int, int> needTrainingByCompetency;
    for (const Assessment& assessment : assessments) {
        totalScoreByCompetency[assessment.competencyId()] += assessment.score();
        assessmentCountByCompetency[assessment.competencyId()] += 1;
        if (assessment.score() < 2) {
            needTrainingByCompetency[assessment.competencyId()] += 1;
        }
    }

    for (const Competency& comp : allCompetencies) {
        const int totalScore = totalScoreByCompetency.value(comp.id(), 0);
        const int assessmentCount = assessmentCountByCompetency.value(comp.id(), 0);
        const int needTraining = needTrainingByCompetency.value(comp.id(), 0);

        if (assessmentCount <= 0) {
            continue;
        }

        const double avgScore = static_cast<double>(totalScore) / assessmentCount;
        const double proficiency = (avgScore / 3.0) * 100.0;
        const double importance = comp.calculatedWeight();
        if (importance < 2.5 || proficiency >= 50.0) {
            continue;
        }

        TrainingRecommendation rec;
        rec.competencyName = comp.name();
        rec.engineersNeed = needTraining;
        const double engineerFactor = engineers.isEmpty() ? 0.0 : (static_cast<double>(needTraining) / engineers.size()) * 5.0;
        rec.impactScore = qMin(importance + engineerFactor, 10.0);
        rec.priority = rec.impactScore >= 7.0 ? "urgent" : (rec.impactScore >= 5.0 ? "high" : "medium");
        recommendations.append(rec);
    }

    std::sort(recommendations.begin(), recommendations.end(),
              [](const TrainingRecommendation& a, const TrainingRecommendation& b) {
                  return a.impactScore > b.impactScore;
              });
    return recommendations;
}

QList<ShiftPerformance> computeShiftPerformance(
    const QList<Engineer>& engineers,
    const QHash<QString, QHash<int, int>>& assessmentScores)
{
    QList<ShiftPerformance> shiftPerformance;
    QMap<QString, ShiftPerformance> shiftsMap;

    for (const Engineer& engineer : engineers) {
        QString shift = engineer.shift();
        if (shift.isEmpty()) {
            shift = "Unassigned";
        }
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
        const auto engineerScoresIt = assessmentScores.constFind(engineer.id());
        if (engineerScoresIt != assessmentScores.constEnd()) {
            for (auto it = engineerScoresIt->constBegin(); it != engineerScoresIt->constEnd(); ++it) {
                totalScore += it.value();
                maxScore += 3;
            }
        }
        if (maxScore > 0) {
            shiftsMap[shift].averageCompletion += (totalScore * 100.0) / maxScore;
        }
    }

    for (auto it = shiftsMap.cbegin(); it != shiftsMap.cend(); ++it) {
        ShiftPerformance perf = it.value();
        if (perf.engineerCount > 0) {
            perf.averageCompletion /= perf.engineerCount;
        }
        shiftPerformance.append(perf);
    }

    return shiftPerformance;
}

QMap<QString, double> computeAreaCoverage(
    const QList<Engineer>& engineers,
    const QList<ProductionArea>& areas,
    const QHash<int, QList<Machine>>& machinesByArea,
    const QHash<int, QList<Competency>>& competenciesByMachine,
    const QHash<QString, QHash<int, int>>& assessmentScores)
{
    QMap<QString, double> areaCoverage;
    if (engineers.isEmpty()) {
        return areaCoverage;
    }

    for (const ProductionArea& area : areas) {
        const QList<Machine>& areaMachines = machinesByArea.value(area.id());
        if (areaMachines.isEmpty()) {
            continue;
        }

        double areaTotalCoverage = 0.0;
        int machineCount = 0;
        for (const Machine& machine : areaMachines) {
            const QList<Competency>& machineCompetencies = competenciesByMachine.value(machine.id());
            if (machineCompetencies.isEmpty()) {
                continue;
            }

            int proficientCount = 0;
            for (const Engineer& engineer : engineers) {
                int totalScore = 0;
                int scoredCompetencies = 0;
                const auto engineerScoresIt = assessmentScores.constFind(engineer.id());
                if (engineerScoresIt == assessmentScores.constEnd()) {
                    continue;
                }
                for (const Competency& comp : machineCompetencies) {
                    const auto scoreIt = engineerScoresIt->constFind(comp.id());
                    if (scoreIt != engineerScoresIt->constEnd()) {
                        totalScore += scoreIt.value();
                        scoredCompetencies++;
                    }
                }

                if (scoredCompetencies > 0) {
                    const double avgScore = static_cast<double>(totalScore) / scoredCompetencies;
                    if (avgScore >= 2.0) {
                        proficientCount++;
                    }
                }
            }

            const double machineCoverage = (proficientCount * 100.0) / engineers.size();
            areaTotalCoverage += machineCoverage;
            machineCount++;
        }

        if (machineCount > 0) {
            areaCoverage.insert(area.name(), areaTotalCoverage / machineCount);
        }
    }

    return areaCoverage;
}

QList<EngineerScoreRowData> computeEngineerScoreRows(
    const QList<Engineer>& engineers,
    const QList<ProductionArea>& areas,
    const QHash<int, QList<Machine>>& machinesByArea,
    const QHash<int, QList<Competency>>& competenciesByMachine,
    const QHash<QString, QList<Assessment>>& assessmentsByEngineer)
{
    struct CompetencyMetadata {
        double machineImportance = 1.0;
        double competencyCriticality = 3.0;
    };

    QHash<int, CompetencyMetadata> competencyMeta;
    QSet<int> allCompetencyIds;
    for (const ProductionArea& area : areas) {
        const QList<Machine>& machines = machinesByArea.value(area.id());
        for (const Machine& machine : machines) {
            const double machineImportance = qBound(1.0, static_cast<double>(machine.importance()), 5.0);
            const QList<Competency>& machineCompetencies = competenciesByMachine.value(machine.id());
            for (const Competency& competency : machineCompetencies) {
                allCompetencyIds.insert(competency.id());
                CompetencyMetadata meta = competencyMeta.value(competency.id());
                meta.machineImportance = qMax(meta.machineImportance, machineImportance);
                meta.competencyCriticality = qBound(1.0, competency.calculatedWeight(), 5.0);
                competencyMeta.insert(competency.id(), meta);
            }
        }
    }
    const int totalCompetencyCount = qMax(1, allCompetencyIds.size());

    auto recencyFactorForDate = [](const QDateTime& assessedAt) -> double {
        const int daysOld = assessedAt.daysTo(QDateTime::currentDateTime());
        if (daysOld <= 90) return 1.00;
        if (daysOld <= 180) return 0.85;
        if (daysOld <= 365) return 0.70;
        if (daysOld <= 730) return 0.50;
        return 0.35;
    };

    QList<EngineerScoreRowData> rows;
    rows.reserve(engineers.size());
    for (const Engineer& engineer : engineers) {
        const QList<Assessment> engineerAssessments = assessmentsByEngineer.value(engineer.id());
        if (engineerAssessments.isEmpty()) {
            continue;
        }

        double rawTotal = 0.0;
        int rawCount = 0;
        double weightedCapabilityTotal = 0.0;
        double weightedDenominator = 0.0;
        int assessedCompetencyCount = 0;

        for (const Assessment& assessment : engineerAssessments) {
            const CompetencyMetadata meta = competencyMeta.value(assessment.competencyId(), CompetencyMetadata());
            const double relevanceWeight = (meta.competencyCriticality * 0.7) + (meta.machineImportance * 0.3);
            const double recencyWeight = recencyFactorForDate(assessment.updatedAt());
            const double combinedWeight = relevanceWeight * recencyWeight;
            const double normalizedPercent = (static_cast<double>(assessment.score()) / 3.0) * 100.0;

            weightedCapabilityTotal += normalizedPercent * combinedWeight;
            weightedDenominator += combinedWeight;
            rawTotal += assessment.score();
            rawCount += 1;
            assessedCompetencyCount += 1;
        }

        if (rawCount == 0 || weightedDenominator == 0.0) {
            continue;
        }

        EngineerScoreRowData row;
        row.name = engineer.name();
        row.rawPercent = (rawTotal / rawCount) / 3.0 * 100.0;
        row.weightedCapabilityPercent = weightedCapabilityTotal / weightedDenominator;
        row.confidencePercent = qMin(100.0,
            (static_cast<double>(assessedCompetencyCount) / totalCompetencyCount) * 100.0);
        rows.append(row);
    }

    return rows;
}

void computeAreaProgressData(
    const QList<ProductionArea>& areas,
    const QList<Assessment>& assessments,
    const QHash<int, QList<Machine>>& machinesByArea,
    const QHash<int, QList<Competency>>& competenciesByMachine,
    QStringList& outMonthLabels,
    QHash<int, QVector<double>>& outAreaProgressByAreaId)
{
    const QDate currentMonth = QDate::currentDate();
    const QDate startMonth = QDate(currentMonth.year(), currentMonth.month(), 1).addMonths(-11);

    outMonthLabels.clear();
    for (int i = 0; i < 12; ++i) {
        outMonthLabels << startMonth.addMonths(i).toString("MMM yy");
    }

    QHash<int, int> competencyToAreaId;
    for (const ProductionArea& area : areas) {
        const QList<Machine>& machines = machinesByArea.value(area.id());
        for (const Machine& machine : machines) {
            const QList<Competency>& competencies = competenciesByMachine.value(machine.id());
            for (const Competency& competency : competencies) {
                competencyToAreaId[competency.id()] = area.id();
            }
        }
    }

    struct MonthAggregate {
        double totalScore = 0.0;
        int count = 0;
    };

    QHash<int, QVector<MonthAggregate>> areaMonthly;
    for (const ProductionArea& area : areas) {
        areaMonthly[area.id()] = QVector<MonthAggregate>(12);
    }

    for (const Assessment& assessment : assessments) {
        const int areaId = competencyToAreaId.value(assessment.competencyId(), -1);
        if (areaId < 0) {
            continue;
        }

        const QDate assessmentDate = assessment.updatedAt().date();
        const int monthOffset = (assessmentDate.year() - startMonth.year()) * 12
            + (assessmentDate.month() - startMonth.month());
        if (monthOffset < 0 || monthOffset >= 12) {
            continue;
        }

        MonthAggregate& aggregate = areaMonthly[areaId][monthOffset];
        aggregate.totalScore += assessment.score();
        aggregate.count += 1;
    }

    outAreaProgressByAreaId.clear();
    for (const ProductionArea& area : areas) {
        const QVector<MonthAggregate>& monthly = areaMonthly.value(area.id());
        QVector<double> percentages;
        percentages.reserve(monthly.size());
        for (const MonthAggregate& aggregate : monthly) {
            double percent = 0.0;
            if (aggregate.count > 0) {
                percent = ((aggregate.totalScore / aggregate.count) / 3.0) * 100.0;
            }
            percentages.append(percent);
        }
        outAreaProgressByAreaId[area.id()] = percentages;
    }
}
}

DashboardWidget::DashboardWidget(QWidget* parent)
    : QWidget(parent)
    , criticalAlertsList_(nullptr)
    , machinesAtRiskLabel_(nullptr)
    , criticalCompetenciesLabel_(nullptr)
    , trainingInProgressLabel_(nullptr)
    , promotionReadyLabel_(nullptr)
    , areaFocusCombo_(nullptr)
    , focusTitleLabel_(nullptr)
    , focusDetailLabel_(nullptr)
    , clearFocusButton_(nullptr)
    , selectedAreaId_(0)
    , machineReadinessContainer_(nullptr)
    , shiftComparisonContainer_(nullptr)
    , urgentTrainingList_(nullptr)
    , promotionReadyList_(nullptr)
    , recentActivityList_(nullptr)
    , areaCoverageRadarView_(nullptr)
    , engineerScoresChartView_(nullptr)
    , weightedScoreToggle_(nullptr)
    , topEngineerLabel_(nullptr)
    , worstEngineerLabel_(nullptr)
    , areaProgressChartView_(nullptr)
    , teamKnowledgeRadarView_(nullptr)
    , viewAnalyticsButton_(nullptr)
    , scheduleTrainingButton_(nullptr)
    , assessEngineersButton_(nullptr)
    , exportReportButton_(nullptr)
    , lastUpdateLabel_(nullptr)
    , refreshButton_(nullptr)
    , isFirstShow_(true)
    , isLoading_(false)
    , waitingForCacheWarmup_(false)
    , showWeightedScores_(false)
    , metricsWatcher_(new QFutureWatcher<MetricsComputationResult>(this))
    , lastDataFetchMs_(0)
    , lastComputeMs_(0)
{
    setupUI();
    connect(metricsWatcher_, &QFutureWatcher<MetricsComputationResult>::finished,
            this, &DashboardWidget::onMetricsComputationFinished);
    Logger::instance().info("DashboardWidget", "Dashboard widget initialized");
}

DashboardWidget::~DashboardWidget()
{
}

void DashboardWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (isFirstShow_) {
        isFirstShow_ = false;
        QTimer::singleShot(0, this, &DashboardWidget::loadStatisticsDeferred);
    }
}

void DashboardWidget::refresh()
{
    loadStatisticsDeferred();
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
    subtitleLabel->setStyleSheet(QString("color: %1;")
        .arg(StyleManager::instance().getColor("textSecondary").name()));
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    AptitudeLogoWidget* logoWidget = new AptitudeLogoWidget(this);
    logoWidget->setSize(120);
    headerLayout->addWidget(logoWidget);

    mainLayout->addLayout(headerLayout);

    // Refresh button in header
    refreshButton_ = new QPushButton("Refresh", this);
    QFont buttonFont = refreshButton_->font();
    buttonFont.setPointSize(14);
    buttonFont.setWeight(QFont::DemiBold);
    refreshButton_->setFont(buttonFont);
    refreshButton_->setMinimumHeight(40);
    refreshButton_->setMinimumWidth(120);
    refreshButton_->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));
    connect(refreshButton_, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton_);

    mainLayout->addSpacing(16);

    // Interactive context replaces the four static score cards.
    setupInteractiveFocusSection(mainLayout);

    // Section 2: Production Area Coverage
    setupAreaCoverageRadarSection(mainLayout);

    // Section 3: Team Knowledge Radar
    setupTeamKnowledgeRadarSection(mainLayout);

    // Section 4: Engineer Scores
    setupEngineerScoresSection(mainLayout);

    // Section 5: 12-Month Area Progress
    setupAreaProgressSection(mainLayout);

    // Section 6: Recent Activity
    setupRecentActivitySection(mainLayout);

    // Footer
    lastUpdateLabel_ = new QLabel("Loading dashboard...", this);
    lastUpdateLabel_->setStyleSheet(QString("color: %1;")
        .arg(StyleManager::instance().getColor("textSecondary").name()));
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
    QLabel* sectionTitle = new QLabel("Critical Alerts", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* alertsBox = new QGroupBox(this);
    alertsBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-left: 4px solid %2;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(
        StyleManager::instance().getColor("border").name(),
        StyleManager::instance().getColor("danger").name()
    ));

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
            "    border: 1px solid %1;"
            "    border-left: 4px solid %1;"
            "    border-radius: 10px;"
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
        titleLabel->setStyleSheet(QString("color: %1;")
            .arg(StyleManager::instance().getColor("textSecondary").name()));
        cardLayout->addWidget(titleLabel);

        valueLabel = new QLabel("0", this);
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(34);
        valueFont.setBold(true);
        valueLabel->setFont(valueFont);
        valueLabel->setStyleSheet(QString("color: %1;")
            .arg(StyleManager::instance().getColor("accent").name()));
        cardLayout->addWidget(valueLabel);

        QLabel* subtitleLabel = new QLabel(subtitle, this);
        QFont subFont = subtitleLabel->font();
        subFont.setPointSize(11);
        subtitleLabel->setFont(subFont);
        subtitleLabel->setStyleSheet(QString("color: %1;")
            .arg(StyleManager::instance().getColor("textSecondary").name()));
        cardLayout->addWidget(subtitleLabel);

        cardLayout->addStretch();
        return card;
    };

    QWidget* card1 = createCard("MACHINES AT RISK", machinesAtRiskLabel_, "under 50% coverage",
        StyleManager::instance().getColor("danger").name());
    QWidget* card2 = createCard("CRITICAL COMPETENCIES", criticalCompetenciesLabel_, "need more staff",
        StyleManager::instance().getColor("warning").name());
    QWidget* card3 = createCard("TRAINING IN PROGRESS", trainingInProgressLabel_, "assessments this month",
        StyleManager::instance().getColor("primary").name());
    QWidget* card4 = createCard("READY FOR PROMOTION", promotionReadyLabel_, "engineers >90%",
        StyleManager::instance().getColor("success").name());

    cardsGrid->addWidget(card1, 0, 0);
    cardsGrid->addWidget(card2, 0, 1);
    cardsGrid->addWidget(card3, 0, 2);
    cardsGrid->addWidget(card4, 0, 3);

    mainLayout->addLayout(cardsGrid);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupInteractiveFocusSection(QVBoxLayout* mainLayout)
{
    QFrame* focusPanel = new QFrame(this);
    focusPanel->setObjectName("dashboardFocusPanel");
    focusPanel->setStyleSheet(
        "QFrame#dashboardFocusPanel{background:#102a43;border:1px solid #1f4c73;"
        "border-radius:14px;}"
        "QComboBox{background:white;color:#0f172a;border:1px solid #b8c9da;border-radius:8px;"
        "padding:9px 12px;min-width:260px;}"
        "QPushButton{background:#e8f2fb;color:#123b5d;border:0;border-radius:8px;"
        "padding:9px 16px;font-weight:700;}"
        "QPushButton:hover{background:white;}");
    QVBoxLayout* panelLayout = new QVBoxLayout(focusPanel);
    panelLayout->setContentsMargins(22, 18, 22, 18);
    panelLayout->setSpacing(12);

    QHBoxLayout* controls = new QHBoxLayout();
    QLabel* exploreLabel = new QLabel("EXPLORE THE OPERATION", focusPanel);
    exploreLabel->setStyleSheet("color:#7dd3fc;font-size:11px;font-weight:800;letter-spacing:1px;");
    areaFocusCombo_ = new QComboBox(focusPanel);
    areaFocusCombo_->addItem("All production areas", 0);
    clearFocusButton_ = new QPushButton("Reset view", focusPanel);
    controls->addWidget(exploreLabel);
    controls->addStretch();
    controls->addWidget(areaFocusCombo_);
    controls->addWidget(clearFocusButton_);
    panelLayout->addLayout(controls);

    focusTitleLabel_ = new QLabel("Team capability at a glance", focusPanel);
    QFont titleFont = focusTitleLabel_->font();
    titleFont.setPointSize(21);
    titleFont.setBold(true);
    focusTitleLabel_->setFont(titleFont);
    focusTitleLabel_->setStyleSheet("color:white;");
    panelLayout->addWidget(focusTitleLabel_);

    focusDetailLabel_ = new QLabel(
        "Click a production area on the radar or select one above to coordinate the dashboard.", focusPanel);
    focusDetailLabel_->setWordWrap(true);
    focusDetailLabel_->setStyleSheet("color:#c9d9e8;font-size:13px;");
    panelLayout->addWidget(focusDetailLabel_);

    connect(areaFocusCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DashboardWidget::onAreaFocusChanged);
    connect(clearFocusButton_, &QPushButton::clicked, this, &DashboardWidget::clearDashboardFocus);

    mainLayout->addWidget(focusPanel);
    mainLayout->addSpacing(8);
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
    readinessBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

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
    shiftBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

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
    QGroupBox* urgentBox = new QGroupBox("Urgent Training", this);
    urgentBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-left: 4px solid %2;"
        "    border-radius: 10px;"
        "    padding: 16px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
    ).arg(
        StyleManager::instance().getColor("border").name(),
        StyleManager::instance().getColor("warning").name()
    ));

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
    QGroupBox* promotionBox = new QGroupBox("Promotion Ready", this);
    promotionBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-left: 4px solid %2;"
        "    border-radius: 10px;"
        "    padding: 16px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
    ).arg(
        StyleManager::instance().getColor("border").name(),
        StyleManager::instance().getColor("success").name()
    ));

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
    activityBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

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

void DashboardWidget::setupAreaCoverageRadarSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Production Area Coverage", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* radarBox = new QGroupBox(this);
    radarBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

    QVBoxLayout* radarLayout = new QVBoxLayout(radarBox);
    areaCoverageRadarView_ = new QChartView(this);
    areaCoverageRadarView_->setRenderHint(QPainter::Antialiasing);
    areaCoverageRadarView_->setMinimumHeight(360);
    areaCoverageRadarView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    radarLayout->addWidget(areaCoverageRadarView_);

    mainLayout->addWidget(radarBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupEngineerScoresSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Engineer Performance", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* scoreBox = new QGroupBox(this);
    scoreBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 16px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

    QVBoxLayout* scoreLayout = new QVBoxLayout(scoreBox);
    scoreLayout->setSpacing(12);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    weightedScoreToggle_ = new QCheckBox("Weighted Score", this);
    weightedScoreToggle_->setChecked(showWeightedScores_);
    weightedScoreToggle_->setStyleSheet(QString("color: %1; font-weight: 700;")
        .arg(StyleManager::instance().getColor("accent").name()));
    connect(weightedScoreToggle_, &QCheckBox::toggled, this, &DashboardWidget::onWeightedScoreToggled);
    controlsLayout->addWidget(weightedScoreToggle_);
    controlsLayout->addStretch();
    scoreLayout->addLayout(controlsLayout);

    QHBoxLayout* summaryLayout = new QHBoxLayout();
    topEngineerLabel_ = new QLabel("Top Engineer: --", this);
    worstEngineerLabel_ = new QLabel("Needs Support: --", this);
    topEngineerLabel_->setStyleSheet(QString("color: %1; font-weight: 700;")
        .arg(StyleManager::instance().getColor("success").name()));
    worstEngineerLabel_->setStyleSheet(QString("color: %1; font-weight: 700;")
        .arg(StyleManager::instance().getColor("danger").name()));
    summaryLayout->addWidget(topEngineerLabel_);
    summaryLayout->addSpacing(24);
    summaryLayout->addWidget(worstEngineerLabel_);
    summaryLayout->addStretch();
    scoreLayout->addLayout(summaryLayout);

    engineerScoresChartView_ = new QChartView(this);
    engineerScoresChartView_->setRenderHint(QPainter::Antialiasing);
    engineerScoresChartView_->setMinimumHeight(320);
    engineerScoresChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scoreLayout->addWidget(engineerScoresChartView_);

    mainLayout->addWidget(scoreBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupTeamKnowledgeRadarSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Team Knowledge Radar", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QLabel* subtitle = new QLabel("Average score across all engineers by discipline (Mechanical, Electrical, Software)", this);
    subtitle->setStyleSheet(QString("color: %1;")
        .arg(StyleManager::instance().getColor("textSecondary").name()));
    mainLayout->addWidget(subtitle);

    QGroupBox* radarBox = new QGroupBox(this);
    radarBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

    QVBoxLayout* radarLayout = new QVBoxLayout(radarBox);
    teamKnowledgeRadarView_ = new QChartView(this);
    teamKnowledgeRadarView_->setRenderHint(QPainter::Antialiasing);
    teamKnowledgeRadarView_->setMinimumHeight(340);
    teamKnowledgeRadarView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    radarLayout->addWidget(teamKnowledgeRadarView_);

    mainLayout->addWidget(radarBox);
    mainLayout->addSpacing(24);
}

void DashboardWidget::setupAreaProgressSection(QVBoxLayout* mainLayout)
{
    QLabel* sectionTitle = new QLabel("Production Area Progress (12 Months)", this);
    QFont titleFont = sectionTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    sectionTitle->setFont(titleFont);
    mainLayout->addWidget(sectionTitle);

    QGroupBox* progressBox = new QGroupBox(this);
    progressBox->setStyleSheet(QString(
        "QGroupBox {"
        "    border: 1px solid %1;"
        "    border-radius: 10px;"
        "    padding: 16px;"
        "}"
    ).arg(StyleManager::instance().getColor("border").name()));

    QVBoxLayout* progressLayout = new QVBoxLayout(progressBox);
    areaProgressChartView_ = new QChartView(this);
    areaProgressChartView_->setRenderHint(QPainter::Antialiasing);
    areaProgressChartView_->setMinimumHeight(360);
    areaProgressChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    progressLayout->addWidget(areaProgressChartView_);

    mainLayout->addWidget(progressBox);
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

    auto createActionButton = [this](const QString& text, const QString& type) -> QPushButton* {
        QPushButton* btn = new QPushButton(text, this);
        QFont btnFont = btn->font();
        btnFont.setPointSize(14);
        btnFont.setWeight(QFont::DemiBold);
        btn->setFont(btnFont);
        btn->setMinimumHeight(50);
        btn->setStyleSheet(StyleManager::instance().getButtonStyle(type));
        return btn;
    };

    viewAnalyticsButton_ = createActionButton("View Full Analytics", "primary");
    scheduleTrainingButton_ = createActionButton("Schedule Training", "success");
    assessEngineersButton_ = createActionButton("Assess Engineers", "secondary");
    exportReportButton_ = createActionButton("Export Report", "danger");

    actionsRow->addWidget(viewAnalyticsButton_);
    actionsRow->addWidget(scheduleTrainingButton_);
    actionsRow->addWidget(assessEngineersButton_);
    actionsRow->addWidget(exportReportButton_);

    mainLayout->addLayout(actionsRow);
    mainLayout->addSpacing(24);
}

void DashboardWidget::loadStatistics()
{
    if (isLoading_) {
        return;
    }
    refreshTimer_.restart();

    DataCache& cache = DataCache::instance();
    if (!cache.isLoaded()) {
        if (!cache.isLoading()) {
            cache.loadAsync();
        }

        if (lastUpdateLabel_) {
            lastUpdateLabel_->setText("Warming data cache...");
        }

        if (!waitingForCacheWarmup_) {
            waitingForCacheWarmup_ = true;
            QTimer::singleShot(150, this, [this]() {
                waitingForCacheWarmup_ = false;
                loadStatisticsDeferred();
            });
        }
        return;
    }

    waitingForCacheWarmup_ = false;
    isLoading_ = true;
    if (refreshButton_) {
        refreshButton_->setEnabled(false);
    }

    Logger::instance().info("DashboardWidget", "Loading dashboard statistics...");
    QElapsedTimer dataFetchTimer;
    dataFetchTimer.start();

    // Build per-refresh data cache once, then reuse in all calculations below.
    cachedEngineers_ = engineerRepo_.findAll();
    cachedAssessments_ = assessmentRepo_.findAll();
    cachedAreas_ = cache.getAreas();
    machinesByArea_.clear();
    competenciesByMachine_.clear();
    assessmentScoreByEngineerAndCompetency_.clear();
    assessmentsByEngineer_.clear();

    for (const ProductionArea& area : cachedAreas_) {
        const QList<Machine> areaMachines = cache.getMachinesByArea(area.id());
        machinesByArea_[area.id()] = areaMachines;
        for (const Machine& machine : areaMachines) {
            competenciesByMachine_[machine.id()] = cache.getCompetenciesByMachine(machine.id());
        }
    }
    for (const Assessment& assessment : cachedAssessments_) {
        assessmentScoreByEngineerAndCompetency_[assessment.engineerId()][assessment.competencyId()] = assessment.score();
        assessmentsByEngineer_[assessment.engineerId()].append(assessment);
    }
    lastDataFetchMs_ = dataFetchTimer.elapsed();

    // Offload heavy metric aggregation to a background worker.
    lastUpdateLabel_->setText("Computing dashboard metrics...");
    const QList<Engineer> engineers = cachedEngineers_;
    const QList<Assessment> assessments = cachedAssessments_;
    const QList<ProductionArea> areas = cachedAreas_;
    const QHash<int, QList<Machine>> machinesByArea = machinesByArea_;
    const QHash<int, QList<Competency>> competenciesByMachine = competenciesByMachine_;
    const QHash<QString, QHash<int, int>> assessmentScores = assessmentScoreByEngineerAndCompetency_;
    const QHash<QString, QList<Assessment>> assessmentsByEngineer = assessmentsByEngineer_;
    computeTimer_.restart();

    auto future = QtConcurrent::run([engineers, assessments, areas, machinesByArea, competenciesByMachine, assessmentScores, assessmentsByEngineer]() {
        MetricsComputationResult result;
        result.machineReadiness = computeMachineReadiness(
            engineers, areas, machinesByArea, competenciesByMachine, assessmentScores);
        result.trainingPriorities = computeTrainingPriorities(
            engineers, areas, assessments, machinesByArea, competenciesByMachine);
        result.shiftPerformance = computeShiftPerformance(engineers, assessmentScores);
        result.areaCoverage = computeAreaCoverage(
            engineers, areas, machinesByArea, competenciesByMachine, assessmentScores);
        result.engineerScoreRows = computeEngineerScoreRows(
            engineers, areas, machinesByArea, competenciesByMachine, assessmentsByEngineer);
        computeAreaProgressData(
            areas, assessments, machinesByArea, competenciesByMachine,
            result.areaProgressMonthLabels, result.areaProgressByAreaId);
        return result;
    });
    metricsWatcher_->setFuture(future);
}

void DashboardWidget::loadStatisticsDeferred()
{
    if (isLoading_) {
        return;
    }
    lastUpdateLabel_->setText("Loading dashboard...");
    QTimer::singleShot(0, this, &DashboardWidget::loadStatistics);
}

void DashboardWidget::onMetricsComputationFinished()
{
    const MetricsComputationResult result = metricsWatcher_->result();
    lastComputeMs_ = computeTimer_.elapsed();
    machineReadinessCache_ = result.machineReadiness;
    trainingPrioritiesCache_ = result.trainingPriorities;
    shiftPerformanceCache_ = result.shiftPerformance;
    areaCoverageCache_ = result.areaCoverage;
    engineerScoreRowsCache_ = result.engineerScoreRows;
    areaProgressMonthLabelsCache_ = result.areaProgressMonthLabels;
    areaProgressByAreaIdCache_ = result.areaProgressByAreaId;

    // Fast-first paint: render sections across event-loop ticks.
    lastUpdateLabel_->setText("Loading dashboard panels...");
    auto stageTimes = QSharedPointer<QMap<QString, qint64>>::create();
    QTimer::singleShot(0, this, [this, stageTimes]() {
        QElapsedTimer stage; stage.start();
        const int previousArea = selectedAreaId_;
        areaFocusCombo_->blockSignals(true);
        areaFocusCombo_->clear();
        areaFocusCombo_->addItem("All production areas", 0);
        for (const ProductionArea& area : cachedAreas_) {
            areaFocusCombo_->addItem(area.name(), area.id());
        }
        const int restoreIndex = areaFocusCombo_->findData(previousArea);
        areaFocusCombo_->setCurrentIndex(restoreIndex >= 0 ? restoreIndex : 0);
        areaFocusCombo_->blockSignals(false);
        updateInteractiveFocusPanel();
        (*stageTimes)["production_health"] = stage.elapsed();
        QTimer::singleShot(0, this, [this, stageTimes]() {
            QElapsedTimer stage; stage.start();
            updateAreaCoverageRadar();
            (*stageTimes)["area_coverage_radar"] = stage.elapsed();
            QTimer::singleShot(0, this, [this, stageTimes]() {
                QElapsedTimer stage; stage.start();
                updateTeamKnowledgeRadar();
                (*stageTimes)["team_knowledge_radar"] = stage.elapsed();
                QTimer::singleShot(0, this, [this, stageTimes]() {
                    QElapsedTimer stage; stage.start();
                    updateEngineerScoresChart();
                    (*stageTimes)["engineer_scores_chart"] = stage.elapsed();
                    QTimer::singleShot(0, this, [this, stageTimes]() {
                        QElapsedTimer stage; stage.start();
                        updateAreaProgressChart();
                        (*stageTimes)["area_progress_chart"] = stage.elapsed();
                        QTimer::singleShot(0, this, [this, stageTimes]() {
                            QElapsedTimer stage; stage.start();
                            updateRecentActivity();
                            (*stageTimes)["recent_activity"] = stage.elapsed();

                            QString timestamp = QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP");
                            lastUpdateLabel_->setText("Last updated: " + timestamp);
                            const qint64 totalMs = refreshTimer_.elapsed();
                            const qint64 renderMs = qMax<qint64>(0, totalMs - lastDataFetchMs_ - lastComputeMs_);
                            Logger::instance().info(
                                "DashboardPerf",
                                QString("timing_ms total=%1 data_fetch=%2 compute=%3 render=%4 "
                                        "production_health=%5 area_coverage_radar=%6 team_knowledge_radar=%7 "
                                        "engineer_scores_chart=%8 area_progress_chart=%9 recent_activity=%10")
                                    .arg(totalMs)
                                    .arg(lastDataFetchMs_)
                                    .arg(lastComputeMs_)
                                    .arg(renderMs)
                                    .arg(stageTimes->value("production_health"))
                                    .arg(stageTimes->value("area_coverage_radar"))
                                    .arg(stageTimes->value("team_knowledge_radar"))
                                    .arg(stageTimes->value("engineer_scores_chart"))
                                    .arg(stageTimes->value("area_progress_chart"))
                                    .arg(stageTimes->value("recent_activity")));
                            Logger::instance().info("DashboardWidget", "Dashboard statistics loaded");
                            if (refreshButton_) {
                                refreshButton_->setEnabled(true);
                            }
                            isLoading_ = false;
                        });
                    });
                });
            });
        });
    });
}

void DashboardWidget::updateCriticalAlerts()
{
    criticalAlertsList_->clear();

    const QList<MachineReadiness>& machineReadiness = machineReadinessCache_;
    const QList<TrainingRecommendation>& trainingPriorities = trainingPrioritiesCache_;

    int alertCount = 0;
    const int maxAlerts = 3;

    // Critical machine coverage alerts
    for (const MachineReadiness& machine : machineReadiness) {
        if (alertCount >= maxAlerts) break;

        if (machine.coveragePercent < 30) {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("[Critical] %1: Only %2% coverage\nAction: Train %3 engineers to reach 70%% coverage")
                .arg(machine.machineName)
                .arg(QString::number(machine.coveragePercent, 'f', 0))
                .arg(qMax(1, int((machine.totalEngineers * 0.7) - machine.proficientCount)));
            item->setText(text);
            item->setBackground(QBrush(StyleManager::instance().getColor("danger").lighter(180)));
            item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
            criticalAlertsList_->addItem(item);
            alertCount++;
        } else if (machine.expertCount <= 1 && machine.importance >= 2) {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("[Warning] %1: Single point of failure (%2 expert%3)\nAction: Cross-train 2 backup engineers immediately")
                .arg(machine.machineName)
                .arg(machine.expertCount)
                .arg(machine.expertCount == 1 ? "" : "s");
            item->setText(text);
            item->setBackground(QBrush(StyleManager::instance().getColor("warning").lighter(175)));
            item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
            criticalAlertsList_->addItem(item);
            alertCount++;
        }
    }

    // High impact training opportunities
    if (alertCount < maxAlerts && !trainingPriorities.isEmpty()) {
        const TrainingRecommendation& top = trainingPriorities.first();
        if (top.priority == "urgent") {
            QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
            QString text = QString("[Opportunity] Train %1 engineers in '%2'\nImpact: %3/10 - Significant production capability gain")
                .arg(top.engineersNeed)
                .arg(top.competencyName)
                .arg(QString::number(top.impactScore, 'f', 1));
            item->setText(text);
            item->setBackground(QBrush(StyleManager::instance().getColor("primary").lighter(185)));
            item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
            criticalAlertsList_->addItem(item);
        }
    }

    if (criticalAlertsList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(criticalAlertsList_);
        item->setText("No critical alerts\nAll systems are operating within safe parameters.");
        item->setBackground(QBrush(StyleManager::instance().getColor("success").lighter(180)));
        item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
        QFont font = item->font();
        font.setPointSize(14);
        font.setBold(true);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void DashboardWidget::updateProductionHealth()
{
    const QList<MachineReadiness>& machineReadiness = machineReadinessCache_;
    const QList<TrainingRecommendation>& trainingPriorities = trainingPrioritiesCache_;

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
    int recentAssessments = 0;
    QDateTime thirtyDaysAgo = QDateTime::currentDateTime().addDays(-30);
    for (const Assessment& assessment : cachedAssessments_) {
        if (assessment.updatedAt() >= thirtyDaysAgo) {
            recentAssessments++;
        }
    }
    trainingInProgressLabel_->setText(QString::number(recentAssessments));

    // Card 4: Ready for promotion (>90% proficiency)
    int promotionReady = 0;
    for (const Engineer& engineer : cachedEngineers_) {
        int totalScore = 0;
        int maxScore = 0;
        const auto engineerScoresIt = assessmentScoreByEngineerAndCompetency_.constFind(engineer.id());
        if (engineerScoresIt != assessmentScoreByEngineerAndCompetency_.constEnd()) {
            for (auto it = engineerScoresIt->constBegin(); it != engineerScoresIt->constEnd(); ++it) {
                totalScore += it.value();
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

    QList<MachineReadiness> machineReadiness = machineReadinessCache_;

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
        QLabel* nameLabel = new QLabel(machine.machineName, this);
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
        if (machine.coveragePercent < 30) barColor = StyleManager::instance().getColor("danger").name();
        else if (machine.coveragePercent < 50) barColor = StyleManager::instance().getColor("warning").name();
        else if (machine.coveragePercent < 70) barColor = StyleManager::instance().getColor("primary").name();
        else barColor = StyleManager::instance().getColor("success").name();

        progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: 1px solid %2;"
            "    border-radius: 4px;"
            "    text-align: center;"
            "    background-color: %3;"
            "    color: %4;"
            "    font-weight: bold;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 3px;"
            "}"
        ).arg(
            barColor,
            StyleManager::instance().getColor("border").name(),
            StyleManager::instance().getColor("alternateBackground").name(),
            StyleManager::instance().getColor("accent").name()
        ));

        rowLayout->addWidget(progressBar);

        // Coverage stats
        QLabel* statsLabel = new QLabel(QString("%1/%2 proficient, %3 experts")
            .arg(machine.proficientCount)
            .arg(machine.totalEngineers)
            .arg(machine.expertCount), this);
        QFont statsFont = statsLabel->font();
        statsFont.setPointSize(12);
        statsLabel->setFont(statsFont);
        statsLabel->setStyleSheet(QString("color: %1;")
            .arg(StyleManager::instance().getColor("textSecondary").name()));
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

    QList<ShiftPerformance> shiftPerformance = shiftPerformanceCache_;

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
        QLabel* nameLabel = new QLabel(shift.shiftName, this);
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

        QString barColor = (i == 0)
            ? StyleManager::instance().getColor("success").name()
            : (i == shiftPerformance.size() - 1)
                ? StyleManager::instance().getColor("warning").name()
                : StyleManager::instance().getColor("primary").name();

        progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: 1px solid %2;"
            "    border-radius: 4px;"
            "    text-align: center;"
            "    background-color: %3;"
            "    color: %4;"
            "    font-weight: bold;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 3px;"
            "}"
        ).arg(
            barColor,
            StyleManager::instance().getColor("border").name(),
            StyleManager::instance().getColor("alternateBackground").name(),
            StyleManager::instance().getColor("accent").name()
        ));

        rowLayout->addWidget(progressBar);

        // Engineer count
        QLabel* statsLabel = new QLabel(QString("%1 engineers").arg(shift.engineerCount), this);
        QFont statsFont = statsLabel->font();
        statsFont.setPointSize(12);
        statsLabel->setFont(statsFont);
        statsLabel->setStyleSheet(QString("color: %1;")
            .arg(StyleManager::instance().getColor("textSecondary").name()));
        statsLabel->setMinimumWidth(120);
        rowLayout->addWidget(statsLabel);

        rowLayout->addStretch();

        shiftComparisonContainer_->layout()->addWidget(row);
    }

    // Add gap analysis
    if (shiftPerformance.size() > 1) {
        double gap = shiftPerformance.first().averageCompletion - shiftPerformance.last().averageCompletion;
        if (gap > 10.0) {
            QLabel* gapLabel = new QLabel(QString("\nPerformance gap: %1% between best and worst shifts - consider cross-shift training")
                .arg(QString::number(gap, 'f', 0)), this);
            QFont font = gapLabel->font();
            font.setPointSize(12);
            font.setBold(true);
            gapLabel->setFont(font);
            gapLabel->setStyleSheet(QString("color: %1; padding: 10px;")
                .arg(StyleManager::instance().getColor("warning").name()));
            shiftComparisonContainer_->layout()->addWidget(gapLabel);
        }
    }
}

void DashboardWidget::updateTrainingPipeline()
{
    urgentTrainingList_->clear();
    promotionReadyList_->clear();

    QList<TrainingRecommendation> trainingPriorities = trainingPrioritiesCache_;

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
            item->setBackground(QBrush(StyleManager::instance().getColor("warning").lighter(175)));
            item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
            urgentTrainingList_->addItem(item);
            count++;
        }
    }

    if (urgentTrainingList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(urgentTrainingList_);
        item->setText("No urgent training needs\nAll critical competencies are adequately staffed.");
        item->setBackground(QBrush(StyleManager::instance().getColor("success").lighter(180)));
        item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
    }

    // Promotion Ready (engineers >90%)
    struct EngineerScore {
        QString name;
        double proficiency;
    };
    QList<EngineerScore> topEngineers;

    for (const Engineer& engineer : cachedEngineers_) {
        int totalScore = 0;
        int maxScore = 0;
        const auto engineerScoresIt = assessmentScoreByEngineerAndCompetency_.constFind(engineer.id());
        if (engineerScoresIt != assessmentScoreByEngineerAndCompetency_.constEnd()) {
            for (auto it = engineerScoresIt->constBegin(); it != engineerScoresIt->constEnd(); ++it) {
                totalScore += it.value();
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
        item->setBackground(QBrush(StyleManager::instance().getColor("success").lighter(180)));
        item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
        promotionReadyList_->addItem(item);
    }

    if (promotionReadyList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(promotionReadyList_);
        item->setText("No engineers currently at\npromotion threshold (>90%)");
        item->setBackground(QBrush(StyleManager::instance().getColor("alternateBackground")));
        item->setForeground(QBrush(StyleManager::instance().getColor("textSecondary")));
    }
}

void DashboardWidget::updateRecentActivity()
{
    recentActivityList_->clear();

    QDateTime sevenDaysAgo = QDateTime::currentDateTime().addDays(-7);

    // Count recent assessments
    int recentAssessments = 0;
    for (const Assessment& assessment : cachedAssessments_) {
        if (assessment.updatedAt() >= sevenDaysAgo) {
            recentAssessments++;
        }
    }

    if (recentAssessments > 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText(QString("%1 new assessments completed").arg(recentAssessments));
        item->setBackground(QBrush(StyleManager::instance().getColor("primary").lighter(185)));
        item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
        recentActivityList_->addItem(item);
    }

    // Count new engineers
    int newEngineers = 0;
    for (const Engineer& engineer : cachedEngineers_) {
        if (engineer.createdAt() >= sevenDaysAgo) {
            newEngineers++;
        }
    }

    if (newEngineers > 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText(QString("%1 new engineer%2 onboarded")
            .arg(newEngineers)
            .arg(newEngineers == 1 ? "" : "s"));
        item->setBackground(QBrush(StyleManager::instance().getColor("success").lighter(180)));
        item->setForeground(QBrush(StyleManager::instance().getColor("accent")));
        recentActivityList_->addItem(item);
    }

    if (recentActivityList_->count() == 0) {
        QListWidgetItem* item = new QListWidgetItem(recentActivityList_);
        item->setText("No recent activity in the last 7 days");
        item->setBackground(QBrush(StyleManager::instance().getColor("alternateBackground")));
        item->setForeground(QBrush(StyleManager::instance().getColor("textSecondary")));
    }
}

void DashboardWidget::updateAreaCoverageRadar()
{
    if (!areaCoverageRadarView_) {
        return;
    }

    QMap<QString, double> areaCoverage = areaCoverageCache_;

    QPolarChart* chart = new QPolarChart();
    chart->setTitle("Coverage by Production Area");
    chart->setAnimationOptions(QPolarChart::NoAnimation);
    chart->legend()->setVisible(false);

    if (areaCoverage.isEmpty()) {
        chart->setTitle("Coverage by Production Area (No Data)");
        areaCoverageRadarView_->setChart(chart);
        return;
    }

    QStringList labels = areaCoverage.keys();
    std::sort(labels.begin(), labels.end());

    QLineSeries* series = new QLineSeries();
    QList<double> values;
    for (int i = 0; i < labels.size(); ++i) {
        const double value = areaCoverage.value(labels[i], 0.0);
        series->append(i, value);
        values.append(value);
    }
    if (!values.isEmpty()) {
        series->append(values.size(), values.first());
    }

    QLineSeries* lowerSeries = new QLineSeries();
    for (int i = 0; i <= values.size(); ++i) {
        lowerSeries->append(i, 0.0);
    }

    QAreaSeries* areaSeries = new QAreaSeries(series, lowerSeries);
    const QColor primary = StyleManager::instance().getColor("primary");
    QLinearGradient gradient(0, 0, 0, 360);
    gradient.setColorAt(0.0, QColor(primary.red(), primary.green(), primary.blue(), 170));
    gradient.setColorAt(1.0, QColor(primary.red(), primary.green(), primary.blue(), 45));
    areaSeries->setBrush(gradient);
    QPen pen(primary);
    pen.setWidth(3);
    areaSeries->setPen(pen);
    chart->addSeries(areaSeries);

    QScatterSeries* clickablePoints = new QScatterSeries();
    clickablePoints->setName("Select area");
    clickablePoints->setMarkerSize(16.0);
    clickablePoints->setColor(QColor("#0EA5E9"));
    clickablePoints->setBorderColor(QColor("#075985"));
    for (int i = 0; i < values.size(); ++i) {
        clickablePoints->append(i, values[i]);
    }
    chart->addSeries(clickablePoints);

    QScatterSeries* selectedPoint = nullptr;
    if (selectedAreaId_ > 0) {
        for (int i = 0; i < labels.size(); ++i) {
            for (const ProductionArea& area : cachedAreas_) {
                if (area.id() == selectedAreaId_ && area.name() == labels[i]) {
                    selectedPoint = new QScatterSeries();
                    selectedPoint->setMarkerSize(26.0);
                    selectedPoint->setColor(QColor("#FF5D67"));
                    selectedPoint->setBorderColor(Qt::white);
                    selectedPoint->append(i, values[i]);
                    chart->addSeries(selectedPoint);
                    break;
                }
            }
        }
    }

    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < labels.size(); ++i) {
        QString axisLabel = labels[i];
        if (axisLabel.length() > 14) {
            axisLabel = axisLabel.left(12) + "..";
        }
        angularAxis->append(axisLabel, i);
    }
    angularAxis->setRange(0, labels.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 100);
    radialAxis->setTickCount(6);
    radialAxis->setLabelFormat("%.0f%%");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    areaSeries->attachAxis(angularAxis);
    areaSeries->attachAxis(radialAxis);
    clickablePoints->attachAxis(angularAxis);
    clickablePoints->attachAxis(radialAxis);
    if (selectedPoint) {
        selectedPoint->attachAxis(angularAxis);
        selectedPoint->attachAxis(radialAxis);
    }

    connect(clickablePoints, &QScatterSeries::clicked, this,
            [this, labels](const QPointF& point) {
                const int index = qRound(point.x());
                if (index < 0 || index >= labels.size()) return;
                for (const ProductionArea& area : cachedAreas_) {
                    if (area.name() == labels[index]) {
                        selectAreaFocus(area.id());
                        return;
                    }
                }
            });

    areaCoverageRadarView_->setChart(chart);
}

void DashboardWidget::updateEngineerScoresChart()
{
    if (!engineerScoresChartView_) {
        return;
    }

    QList<EngineerScoreRowData> rows = engineerScoreRowsCache_;

    QChart* chart = new QChart();
    chart->legend()->setVisible(false);
    chart->setAnimationOptions(QChart::NoAnimation);

    if (rows.isEmpty()) {
        chart->setTitle("Engineer Scores (No Data)");
        engineerScoresChartView_->setChart(chart);
        if (topEngineerLabel_) {
            topEngineerLabel_->setText("Top Engineer: --");
        }
        if (worstEngineerLabel_) {
            worstEngineerLabel_->setText("Needs Support: --");
        }
        return;
    }

    std::sort(rows.begin(), rows.end(), [this](const EngineerScoreRowData& a, const EngineerScoreRowData& b) {
        const double aValue = showWeightedScores_ ? a.weightedCapabilityPercent : a.rawPercent;
        const double bValue = showWeightedScores_ ? b.weightedCapabilityPercent : b.rawPercent;
        if (showWeightedScores_ && qFuzzyCompare(aValue + 1.0, bValue + 1.0)) {
            return a.confidencePercent > b.confidencePercent;
        }
        return aValue > bValue;
    });

    const EngineerScoreRowData& top = rows.first();
    const EngineerScoreRowData& worst = rows.last();
    const double topValue = showWeightedScores_ ? top.weightedCapabilityPercent : top.rawPercent;
    const double worstValue = showWeightedScores_ ? worst.weightedCapabilityPercent : worst.rawPercent;

    if (topEngineerLabel_) {
        topEngineerLabel_->setText(QString("Top Engineer: %1 (%2%, confidence %3%)")
            .arg(top.name)
            .arg(QString::number(topValue, 'f', 1))
            .arg(QString::number(top.confidencePercent, 'f', 0)));
    }
    if (worstEngineerLabel_) {
        worstEngineerLabel_->setText(QString("Needs Support: %1 (%2%, confidence %3%)")
            .arg(worst.name)
            .arg(QString::number(worstValue, 'f', 1))
            .arg(QString::number(worst.confidencePercent, 'f', 0)));
    }

    const int displayCount = qMin(10, rows.size());
    QBarSet* barSet = new QBarSet(showWeightedScores_ ? "Weighted Capability %" : "Raw %");
    barSet->setColor(StyleManager::instance().getColor("primary"));
    QStringList categories;

    for (int i = 0; i < displayCount; ++i) {
        const EngineerScoreRowData& row = rows[i];
        const double value = showWeightedScores_ ? row.weightedCapabilityPercent : row.rawPercent;
        *barSet << value;
        QString label = row.name;
        if (label.length() > 14) {
            label = label.left(12) + "..";
        }
        categories << label;
    }

    QBarSeries* series = new QBarSeries();
    series->append(barSet);
    series->setLabelsVisible(true);
    series->setLabelsFormat("@value%");
    series->setLabelsPrecision(1);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    chart->addSeries(series);

    connect(barSet, &QBarSet::clicked, this, [this, rows, displayCount](int index) {
        if (index < 0 || index >= displayCount) return;
        const EngineerScoreRowData& row = rows[index];
        const double value = showWeightedScores_ ? row.weightedCapabilityPercent : row.rawPercent;
        focusTitleLabel_->setText(row.name);
        focusDetailLabel_->setText(QString(
            "Engineer score %1%  |  Evidence confidence %2%  |  %3")
            .arg(value, 0, 'f', 1)
            .arg(row.confidencePercent, 0, 'f', 0)
            .arg(row.confidencePercent < 70.0
                ? "Complete missing assessments before finalising a development plan."
                : "Open Engineer Development to assign focused machine actions."));
        clearFocusButton_->setEnabled(true);
    });

    chart->setTitle(showWeightedScores_
        ? "Engineer Scores (Weighted: criticality + recency; confidence shown above)"
        : "Engineer Scores (Raw Average)");

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%.0f%%");
    axisY->setTickCount(6);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    engineerScoresChartView_->setChart(chart);
}

void DashboardWidget::updateTeamKnowledgeRadar()
{
    if (!teamKnowledgeRadarView_) {
        return;
    }

    QPolarChart* chart = new QPolarChart();
    chart->setTitle("Team Knowledge by Discipline");
    chart->setAnimationOptions(QPolarChart::NoAnimation);
    chart->legend()->setVisible(false);

    const QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    const QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();
    const QList<CoreSkillAssessment> assessments = coreSkillsRepo_.findAllAssessments();

    if (categories.isEmpty() || skills.isEmpty() || assessments.isEmpty()) {
        chart->setTitle("Team Knowledge by Discipline (No Data)");
        teamKnowledgeRadarView_->setChart(chart);
        return;
    }

    QHash<QString, QString> categoryNameById;
    for (const CoreSkillCategory& category : categories) {
        categoryNameById[category.id()] = category.name();
    }

    QHash<QString, QString> disciplineBySkillId;
    for (const CoreSkill& skill : skills) {
        const QString categoryName = categoryNameById.value(skill.categoryId(), QString());
        const QString categoryId = skill.categoryId().toLower();
        const QString lowerName = categoryName.toLower();

        QString discipline;
        if (lowerName.startsWith("mechanical") || categoryId.contains("mech")) {
            discipline = "Mechanical";
        } else if (lowerName.startsWith("electrical") || categoryId.contains("elec")) {
            discipline = "Electrical";
        } else if (lowerName.startsWith("software") || categoryId.contains("soft")) {
            discipline = "Software";
        } else {
            continue;
        }

        disciplineBySkillId[skill.id()] = discipline;
    }

    QMap<QString, double> scoreTotals = {
        {"Mechanical", 0.0},
        {"Electrical", 0.0},
        {"Software", 0.0}
    };
    QMap<QString, int> scoreCounts = {
        {"Mechanical", 0},
        {"Electrical", 0},
        {"Software", 0}
    };

    for (const CoreSkillAssessment& assessment : assessments) {
        const QString discipline = disciplineBySkillId.value(assessment.skillId());
        if (discipline.isEmpty()) {
            continue;
        }
        scoreTotals[discipline] += assessment.score();
        scoreCounts[discipline] += 1;
    }

    QStringList labels = {"Mechanical", "Electrical", "Software"};
    QLineSeries* series = new QLineSeries();
    QList<double> disciplineValues;
    bool hasData = false;
    for (int i = 0; i < labels.size(); ++i) {
        const QString& label = labels[i];
        double avgScore = 0.0;
        if (scoreCounts[label] > 0) {
            avgScore = scoreTotals[label] / static_cast<double>(scoreCounts[label]);
            hasData = true;
        }
        series->append(i, avgScore);
        disciplineValues.append(avgScore);
    }
    series->append(labels.size(), series->at(0).y());

    if (!hasData) {
        chart->setTitle("Team Knowledge by Discipline (No Data)");
        teamKnowledgeRadarView_->setChart(chart);
        return;
    }

    QLineSeries* lowerSeries = new QLineSeries();
    for (int i = 0; i <= labels.size(); ++i) {
        lowerSeries->append(i, 0.0);
    }

    QAreaSeries* areaSeries = new QAreaSeries(series, lowerSeries);
    const QColor accent = StyleManager::instance().getColor("accent");
    QLinearGradient gradient(0, 0, 0, 340);
    gradient.setColorAt(0.0, QColor(accent.red(), accent.green(), accent.blue(), 170));
    gradient.setColorAt(1.0, QColor(accent.red(), accent.green(), accent.blue(), 45));
    areaSeries->setBrush(gradient);
    QPen pen(accent);
    pen.setWidth(3);
    areaSeries->setPen(pen);
    chart->addSeries(areaSeries);

    QScatterSeries* disciplinePoints = new QScatterSeries();
    disciplinePoints->setMarkerSize(22.0);
    disciplinePoints->setColor(QColor("#FF9F1C"));
    disciplinePoints->setBorderColor(QColor("#9A5A00"));
    for (int i = 0; i < disciplineValues.size(); ++i) {
        disciplinePoints->append(i, disciplineValues[i]);
    }
    chart->addSeries(disciplinePoints);

    QCategoryAxis* angularAxis = new QCategoryAxis();
    angularAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < labels.size(); ++i) {
        angularAxis->append(labels[i], i);
    }
    angularAxis->setRange(0, labels.size());
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis* radialAxis = new QValueAxis();
    radialAxis->setRange(0, 3);
    radialAxis->setTickCount(4);
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    areaSeries->attachAxis(angularAxis);
    areaSeries->attachAxis(radialAxis);
    disciplinePoints->attachAxis(angularAxis);
    disciplinePoints->attachAxis(radialAxis);

    connect(disciplinePoints, &QScatterSeries::clicked, this,
            [this, labels, disciplineValues](const QPointF& point) {
                const int index = qRound(point.x());
                if (index < 0 || index >= labels.size()) return;
                focusTitleLabel_->setText(labels[index] + " core skills");
                focusDetailLabel_->setText(QString(
                    "Team average %1 / 3  |  Click Engineer Development to identify individual gaps and assign actions.")
                    .arg(disciplineValues[index], 0, 'f', 2));
                clearFocusButton_->setEnabled(true);
            });

    teamKnowledgeRadarView_->setChart(chart);
}

void DashboardWidget::updateAreaProgressChart()
{
    if (!areaProgressChartView_) {
        return;
    }

    QChart* chart = new QChart();
    chart->setTitle(selectedAreaId_ > 0
        ? "12-Month Progress: Selected Production Area"
        : "12-Month Progress by Production Area");
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    if (cachedAreas_.isEmpty() || areaProgressMonthLabelsCache_.isEmpty()) {
        chart->setTitle("12-Month Progress by Production Area (No Data)");
        areaProgressChartView_->setChart(chart);
        return;
    }

    const QList<QColor> palette = {
        QColor("#1D4ED8"), QColor("#0EA5A4"), QColor("#F59E0B"),
        QColor("#16A34A"), QColor("#DC2626"), QColor("#7C3AED"),
        QColor("#0891B2"), QColor("#BE123C")
    };

    int colorIndex = 0;
    bool hasSeries = false;
    for (const ProductionArea& area : cachedAreas_) {
        if (selectedAreaId_ > 0 && area.id() != selectedAreaId_) continue;
        QLineSeries* series = new QLineSeries();
        series->setName(area.name());
        const QColor lineColor = palette[colorIndex % palette.size()];
        colorIndex++;

        QPen pen(lineColor);
        pen.setWidth(3);
        series->setPen(pen);

        const QVector<double>& monthlyPercentages = areaProgressByAreaIdCache_.value(area.id());
        for (int month = 0; month < monthlyPercentages.size(); ++month) {
            series->append(month, monthlyPercentages[month]);
        }

        chart->addSeries(series);
        connect(series, &QLineSeries::clicked, this,
                [this, area](const QPointF&) { selectAreaFocus(area.id()); });
        connect(series, &QLineSeries::hovered, this,
                [series](const QPointF&, bool state) {
                    QPen pen = series->pen();
                    pen.setWidth(state ? 6 : 3);
                    series->setPen(pen);
                });
        if (!monthlyPercentages.isEmpty()) {
            hasSeries = true;
        }
    }

    if (!hasSeries) {
        chart->setTitle("12-Month Progress by Production Area (No Data)");
        areaProgressChartView_->setChart(chart);
        return;
    }

    QCategoryAxis* axisX = new QCategoryAxis();
    axisX->setRange(0, 11);
    for (int i = 0; i < areaProgressMonthLabelsCache_.size(); ++i) {
        axisX->append(areaProgressMonthLabelsCache_[i], i);
    }
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTickCount(6);
    axisY->setLabelFormat("%.0f%%");
    chart->addAxis(axisY, Qt::AlignLeft);

    const QList<QAbstractSeries*> allSeries = chart->series();
    for (QAbstractSeries* abstractSeries : allSeries) {
        abstractSeries->attachAxis(axisX);
        abstractSeries->attachAxis(axisY);
    }

    areaProgressChartView_->setChart(chart);
}

// Calculation methods (reused from AnalyticsWidget)

QList<DashboardWidget::MachineReadiness> DashboardWidget::calculateMachineReadiness()
{
    return machineReadinessCache_;
}

QList<DashboardWidget::TrainingRecommendation> DashboardWidget::calculateTrainingPriorities()
{
    return trainingPrioritiesCache_;
}

QList<DashboardWidget::ShiftPerformance> DashboardWidget::calculateShiftPerformance()
{
    return shiftPerformanceCache_;
}

QMap<QString, double> DashboardWidget::calculateProductionAreaCoverage()
{
    return areaCoverageCache_;
}

void DashboardWidget::onAreaFocusChanged(int index)
{
    Q_UNUSED(index);
    selectAreaFocus(areaFocusCombo_ ? areaFocusCombo_->currentData().toInt() : 0);
}

void DashboardWidget::clearDashboardFocus()
{
    selectAreaFocus(0);
}

void DashboardWidget::selectAreaFocus(int areaId)
{
    selectedAreaId_ = areaId;
    if (areaFocusCombo_) {
        areaFocusCombo_->blockSignals(true);
        const int index = areaFocusCombo_->findData(areaId);
        areaFocusCombo_->setCurrentIndex(index >= 0 ? index : 0);
        areaFocusCombo_->blockSignals(false);
    }
    updateInteractiveFocusPanel();
    updateAreaCoverageRadar();
    updateAreaProgressChart();
}

void DashboardWidget::updateInteractiveFocusPanel()
{
    if (!focusTitleLabel_ || !focusDetailLabel_) return;

    if (selectedAreaId_ <= 0) {
        focusTitleLabel_->setText("Team capability at a glance");
        focusDetailLabel_->setText(QString(
            "%1 engineers across %2 production areas. Select an area or click the radar to reveal its weakest machine and isolate its 12-month trend.")
            .arg(cachedEngineers_.size()).arg(cachedAreas_.size()));
        if (clearFocusButton_) clearFocusButton_->setEnabled(false);
        return;
    }

    ProductionArea selectedArea;
    bool areaFound = false;
    for (const ProductionArea& area : cachedAreas_) {
        if (area.id() == selectedAreaId_) {
            selectedArea = area;
            areaFound = true;
            break;
        }
    }
    if (!areaFound) {
        selectedAreaId_ = 0;
        updateInteractiveFocusPanel();
        return;
    }

    MachineReadiness weakestMachine;
    bool machineFound = false;
    const QList<Machine> areaMachines = machinesByArea_.value(selectedAreaId_);
    QSet<int> areaMachineIds;
    for (const Machine& machine : areaMachines) areaMachineIds.insert(machine.id());
    for (const MachineReadiness& readiness : machineReadinessCache_) {
        if (!areaMachineIds.contains(readiness.machineId)) continue;
        if (!machineFound || readiness.coveragePercent < weakestMachine.coveragePercent) {
            weakestMachine = readiness;
            machineFound = true;
        }
    }

    const double areaCoverage = areaCoverageCache_.value(selectedArea.name(), 0.0);
    focusTitleLabel_->setText(selectedArea.name());
    if (machineFound) {
        const QString action = weakestMachine.coveragePercent < 50.0
            ? "Prioritise cross-training and assign two named backups."
            : "Coverage is stable; develop another expert to improve resilience.";
        focusDetailLabel_->setText(QString(
            "Area coverage %1%  |  Weakest machine: %2 at %3%  |  %4")
            .arg(areaCoverage, 0, 'f', 0)
            .arg(weakestMachine.machineName)
            .arg(weakestMachine.coveragePercent, 0, 'f', 0)
            .arg(action));
    } else {
        focusDetailLabel_->setText("No machine competency data is available for this production area.");
    }
    if (clearFocusButton_) clearFocusButton_->setEnabled(true);
}

void DashboardWidget::onRefreshClicked()
{
    loadStatisticsDeferred();
    Logger::instance().info("DashboardWidget", "Dashboard refreshed");
}

void DashboardWidget::onWeightedScoreToggled(bool checked)
{
    showWeightedScores_ = checked;
    updateEngineerScoresChart();
}
