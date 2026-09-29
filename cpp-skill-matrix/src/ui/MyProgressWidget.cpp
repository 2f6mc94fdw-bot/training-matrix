#include "MyProgressWidget.h"
#include "../utils/Logger.h"
#include "../utils/JsonHelper.h"
#include "../core/DataCache.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QShowEvent>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <algorithm>

MyProgressWidget::MyProgressWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , isFirstShow_(true)
    , totalImprovementsLabel_(nullptr)
    , avgGrowthRateLabel_(nullptr)
    , skillsGainedLabel_(nullptr)
    , upcomingCertsLabel_(nullptr)
    , skillProgressChartView_(nullptr)
    , coreSkillProgressChartView_(nullptr)
    , snapshotCombo_(nullptr)
    , snapshotComparisonLabel_(nullptr)
    , changesListWidget_(nullptr)
    , certificationsListWidget_(nullptr)
    , goalsListWidget_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
}

MyProgressWidget::~MyProgressWidget()
{
}

void MyProgressWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (isFirstShow_) {
        isFirstShow_ = false;
        loadProgressData();
    }
}

void MyProgressWidget::setupUI()
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
    QLabel* titleLabel = new QLabel("My Progress", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    refreshButton_ = new QPushButton("Refresh", this);
    connect(refreshButton_, &QPushButton::clicked, this, &MyProgressWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton_);

    mainLayout->addLayout(headerLayout);

    // Progress stats section - 4 stat cards
    QGroupBox* statsGroup = new QGroupBox("Progress Overview", this);
    QHBoxLayout* statsLayout = new QHBoxLayout(statsGroup);
    statsLayout->setSpacing(15);

    // Stat card 1: Total Improvements
    QGroupBox* stat1 = new QGroupBox(this);
    QVBoxLayout* stat1Layout = new QVBoxLayout(stat1);
    QLabel* stat1Title = new QLabel("Total Improvements", this);
    stat1Title->setStyleSheet("font-weight: bold; color: #666;");
    totalImprovementsLabel_ = new QLabel("0", this);
    QFont statFont;
    statFont.setPointSize(24);
    statFont.setBold(true);
    totalImprovementsLabel_->setFont(statFont);
    totalImprovementsLabel_->setStyleSheet("color: #2196F3;");
    stat1Layout->addWidget(stat1Title);
    stat1Layout->addWidget(totalImprovementsLabel_);
    stat1Layout->addStretch();
    statsLayout->addWidget(stat1);

    // Stat card 2: Change vs Last Snapshot
    QGroupBox* stat2 = new QGroupBox(this);
    QVBoxLayout* stat2Layout = new QVBoxLayout(stat2);
    QLabel* stat2Title = new QLabel("Change vs Last Snapshot", this);
    stat2Title->setStyleSheet("font-weight: bold; color: #666;");
    avgGrowthRateLabel_ = new QLabel("0.0 pts", this);
    avgGrowthRateLabel_->setFont(statFont);
    avgGrowthRateLabel_->setStyleSheet("color: #4CAF50;");
    stat2Layout->addWidget(stat2Title);
    stat2Layout->addWidget(avgGrowthRateLabel_);
    stat2Layout->addStretch();
    statsLayout->addWidget(stat2);

    // Stat card 3: Competencies at 2+
    QGroupBox* stat3 = new QGroupBox(this);
    QVBoxLayout* stat3Layout = new QVBoxLayout(stat3);
    QLabel* stat3Title = new QLabel("Competencies at 2+", this);
    stat3Title->setStyleSheet("font-weight: bold; color: #666;");
    skillsGainedLabel_ = new QLabel("0", this);
    skillsGainedLabel_->setFont(statFont);
    skillsGainedLabel_->setStyleSheet("color: #FF9800;");
    stat3Layout->addWidget(stat3Title);
    stat3Layout->addWidget(skillsGainedLabel_);
    stat3Layout->addStretch();
    statsLayout->addWidget(stat3);

    // Stat card 4: Upcoming Certifications
    QGroupBox* stat4 = new QGroupBox(this);
    QVBoxLayout* stat4Layout = new QVBoxLayout(stat4);
    QLabel* stat4Title = new QLabel("Upcoming Certs", this);
    stat4Title->setStyleSheet("font-weight: bold; color: #666;");
    upcomingCertsLabel_ = new QLabel("0", this);
    upcomingCertsLabel_->setFont(statFont);
    upcomingCertsLabel_->setStyleSheet("color: #9C27B0;");
    stat4Layout->addWidget(stat4Title);
    stat4Layout->addWidget(upcomingCertsLabel_);
    stat4Layout->addStretch();
    statsLayout->addWidget(stat4);

    mainLayout->addWidget(statsGroup);

    // Progress charts section
    QGroupBox* chartsGroup = new QGroupBox("Skills Progress Over Time", this);
    QVBoxLayout* chartsLayout = new QVBoxLayout(chartsGroup);

    // Competency Progress Chart
    QLabel* skillChartTitle = new QLabel("Competency Skills Progress", this);
    skillChartTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    skillProgressChartView_ = new QChartView(this);
    skillProgressChartView_->setRenderHint(QPainter::Antialiasing);
    skillProgressChartView_->setMinimumHeight(400);
    skillProgressChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsLayout->addWidget(skillChartTitle);
    chartsLayout->addWidget(skillProgressChartView_, 1);

    // Core Skills Progress Chart
    QLabel* coreChartTitle = new QLabel("Core Skills Progress", this);
    coreChartTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    coreSkillProgressChartView_ = new QChartView(this);
    coreSkillProgressChartView_->setRenderHint(QPainter::Antialiasing);
    coreSkillProgressChartView_->setMinimumHeight(400);
    coreSkillProgressChartView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsLayout->addWidget(coreChartTitle);
    chartsLayout->addWidget(coreSkillProgressChartView_, 1);

    mainLayout->addWidget(chartsGroup);

    // Snapshot comparison section
    QGroupBox* snapshotGroup = new QGroupBox("Snapshot Comparison", this);
    QVBoxLayout* snapshotLayout = new QVBoxLayout(snapshotGroup);

    QHBoxLayout* snapshotSelectLayout = new QHBoxLayout();
    QLabel* snapshotLabel = new QLabel("Compare with snapshot:", this);
    snapshotCombo_ = new QComboBox(this);
    connect(snapshotCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MyProgressWidget::onSnapshotComparisonChanged);
    snapshotSelectLayout->addWidget(snapshotLabel);
    snapshotSelectLayout->addWidget(snapshotCombo_, 1);
    snapshotLayout->addLayout(snapshotSelectLayout);

    snapshotComparisonLabel_ = new QLabel("Select a snapshot to compare", this);
    snapshotComparisonLabel_->setStyleSheet("font-style: italic; color: #666;");
    snapshotLayout->addWidget(snapshotComparisonLabel_);

    changesListWidget_ = new QListWidget(this);
    changesListWidget_->setMaximumHeight(150);
    snapshotLayout->addWidget(changesListWidget_);

    mainLayout->addWidget(snapshotGroup);

    // Two columns: Certifications | Goals
    QHBoxLayout* bottomSection = new QHBoxLayout();

    // Certifications timeline
    QGroupBox* certsGroup = new QGroupBox("Certifications Timeline", this);
    QVBoxLayout* certsLayout = new QVBoxLayout(certsGroup);
    certificationsListWidget_ = new QListWidget(this);
    certificationsListWidget_->setMaximumHeight(200);
    certsLayout->addWidget(certificationsListWidget_);
    bottomSection->addWidget(certsGroup);

    // Goals/Targets
    QGroupBox* goalsGroup = new QGroupBox("Goals & Targets", this);
    QVBoxLayout* goalsLayout = new QVBoxLayout(goalsGroup);
    QLabel* goalsNote = new QLabel("Auto-generated action plan based on current assessments", this);
    goalsNote->setStyleSheet("font-style: italic; color: #667085; font-size: 11px;");
    goalsLayout->addWidget(goalsNote);
    goalsListWidget_ = new QListWidget(this);
    goalsListWidget_->setMaximumHeight(200);
    goalsLayout->addWidget(goalsListWidget_);
    bottomSection->addWidget(goalsGroup);

    mainLayout->addLayout(bottomSection);
    mainLayout->addStretch();

    // Wire up scroll area
    scrollArea->setWidget(scrollContent);
    widgetLayout->addWidget(scrollArea);
}

void MyProgressWidget::loadProgressData()
{
    Logger::instance().info("MyProgressWidget", QString("Loading progress for engineer: %1").arg(engineerId_));

    // Load engineer data
    currentEngineer_ = engineerRepo_.findById(engineerId_);

    // Load only this engineer's data to avoid full-table scans on large datasets.
    assessments_ = assessmentRepo_.findByEngineer(engineerId_);
    coreSkillAssessments_ = coreSkillsRepo_.findAssessmentsByEngineer(engineerId_);

    // Load snapshots
    snapshots_ = snapshotRepo_.findAll();

    // Load certifications
    certifications_ = certificationRepo_.findByEngineer(engineerId_);

    // Update all sections
    createSkillProgressChart();
    createCoreSkillProgressChart();
    updateSnapshotComparison();
    updateCertificationTimeline();
    updateGoalsProgress();

    // Update stats
    totalImprovementsLabel_->setText(QString::number(assessments_.size() + coreSkillAssessments_.size()));
    int proficientCompetencies = 0;
    for (const Assessment& assessment : assessments_) {
        if (assessment.score() >= 2) {
            proficientCompetencies++;
        }
    }
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        if (assessment.score() >= 2) {
            proficientCompetencies++;
        }
    }
    skillsGainedLabel_->setText(QString::number(proficientCompetencies));

    // Count upcoming certifications (no expiry or expiry in future)
    int upcomingCerts = 0;
    QDate today = QDate::currentDate();
    for (const Certification& cert : certifications_) {
        if (!cert.expiryDate().isValid() || cert.expiryDate() >= today) {
            upcomingCerts++;
        }
    }
    upcomingCertsLabel_->setText(QString::number(upcomingCerts));

    // Compare weighted average against the latest snapshot for a meaningful trend stat.
    double currentTotal = 0.0;
    int currentCount = 0;
    for (const Assessment& assessment : assessments_) {
        currentTotal += assessment.score();
        currentCount++;
    }
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        currentTotal += assessment.score();
        currentCount++;
    }

    double currentAverage = (currentCount > 0) ? (currentTotal / static_cast<double>(currentCount)) : 0.0;
    double latestSnapshotAverage = currentAverage;

    if (!snapshots_.isEmpty()) {
        const Snapshot& latestSnapshot = snapshots_.first();
        QJsonDocument doc = QJsonDocument::fromJson(latestSnapshot.data().toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            double snapshotTotal = 0.0;
            int snapshotCount = 0;

            if (obj.contains("assessments")) {
                for (const QJsonValue& val : obj["assessments"].toArray()) {
                    QJsonObject assessment = val.toObject();
                    if (assessment["engineerId"].toString() == engineerId_) {
                        snapshotTotal += assessment["score"].toInt();
                        snapshotCount++;
                    }
                }
            }
            if (obj.contains("coreSkillAssessments")) {
                for (const QJsonValue& val : obj["coreSkillAssessments"].toArray()) {
                    QJsonObject assessment = val.toObject();
                    if (assessment["engineerId"].toString() == engineerId_) {
                        snapshotTotal += assessment["score"].toInt();
                        snapshotCount++;
                    }
                }
            }

            if (snapshotCount > 0) {
                latestSnapshotAverage = snapshotTotal / static_cast<double>(snapshotCount);
            }
        }
    }

    const double delta = currentAverage - latestSnapshotAverage;
    avgGrowthRateLabel_->setText(QString("%1%2 pts")
        .arg(delta >= 0.0 ? "+" : "")
        .arg(QString::number(delta, 'f', 1)));
    avgGrowthRateLabel_->setStyleSheet(delta < 0.0
        ? "color: #F04438;"
        : "color: #16A34A;");
}

void MyProgressWidget::createSkillProgressChart()
{
    QChart* chart = new QChart();
    chart->setTitle("Average Competency Score Over Time (Weighted)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QLineSeries* series = new QLineSeries();
    series->setName("Weighted Avg Score");

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

    QVector<QPointF> points;

    // Current point (WEIGHTED)
    if (!assessments_.isEmpty()) {
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

        double currentAvg = (totalWeights > 0.0) ? (weightedSum / totalWeights) : 0.0;

        QDateTime now = QDateTime::currentDateTime();
        points.append(QPointF(now.toMSecsSinceEpoch(), currentAvg));
    }

    // Parse snapshots for historical data
    for (const Snapshot& snapshot : snapshots_) {
        // Parse JSON data
        QJsonDocument doc = QJsonDocument::fromJson(snapshot.data().toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("assessments")) {
                QJsonArray assessmentsArray = obj["assessments"].toArray();

                double total = 0.0;
                int count = 0;
                for (const QJsonValue& val : assessmentsArray) {
                    QJsonObject assessment = val.toObject();
                    if (assessment["engineerId"].toString() == engineerId_) {
                        total += assessment["score"].toInt();
                        count++;
                    }
                }

                if (count > 0) {
                    double avg = total / count;
                    QDateTime timestamp = snapshot.timestamp();
                    points.append(QPointF(timestamp.toMSecsSinceEpoch(), avg));
                }
            }
        }
    }

    std::sort(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) {
        return a.x() < b.x();
    });
    for (const QPointF& point : points) {
        series->append(point);
    }

    chart->addSeries(series);

    // Configure axes
    QDateTimeAxis* axisX = new QDateTimeAxis();
    axisX->setFormat("MMM yyyy");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 3);
    axisY->setTitleText("Average Score");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    skillProgressChartView_->setChart(chart);
}

void MyProgressWidget::createCoreSkillProgressChart()
{
    QChart* chart = new QChart();
    chart->setTitle("Average Core Skills Score Over Time (Weighted)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QLineSeries* series = new QLineSeries();
    series->setName("Weighted Avg Core Skills Score");

    // Load all core skills to get weights
    QList<CoreSkill> allCoreSkills = coreSkillsRepo_.findAllSkills();

    QVector<QPointF> points;

    // Current point (WEIGHTED)
    if (!coreSkillAssessments_.isEmpty()) {
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

        double currentAvg = (totalWeights > 0.0) ? (weightedSum / totalWeights) : 0.0;

        QDateTime now = QDateTime::currentDateTime();
        points.append(QPointF(now.toMSecsSinceEpoch(), currentAvg));
    }

    // Parse snapshots for historical data
    for (const Snapshot& snapshot : snapshots_) {
        QJsonDocument doc = QJsonDocument::fromJson(snapshot.data().toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("coreSkillAssessments")) {
                QJsonArray coreSkillsArray = obj["coreSkillAssessments"].toArray();

                double total = 0.0;
                int count = 0;
                for (const QJsonValue& val : coreSkillsArray) {
                    QJsonObject assessment = val.toObject();
                    if (assessment["engineerId"].toString() == engineerId_) {
                        total += assessment["score"].toInt();
                        count++;
                    }
                }

                if (count > 0) {
                    double avg = total / count;
                    QDateTime timestamp = snapshot.timestamp();
                    points.append(QPointF(timestamp.toMSecsSinceEpoch(), avg));
                }
            }
        }
    }

    std::sort(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) {
        return a.x() < b.x();
    });
    for (const QPointF& point : points) {
        series->append(point);
    }

    chart->addSeries(series);

    // Configure axes
    QDateTimeAxis* axisX = new QDateTimeAxis();
    axisX->setFormat("MMM yyyy");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 3);
    axisY->setTitleText("Average Score");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    coreSkillProgressChartView_->setChart(chart);
}

void MyProgressWidget::updateSnapshotComparison()
{
    snapshotCombo_->clear();
    snapshotCombo_->addItem("Select a snapshot...");

    for (const Snapshot& snapshot : snapshots_) {
        QString label = QString("%1 - %2")
            .arg(snapshot.timestamp().toString("yyyy-MM-dd HH:mm"))
            .arg(snapshot.description());
        snapshotCombo_->addItem(label, snapshot.id());
    }

    const bool hasSnapshots = snapshotCombo_->count() > 1;
    snapshotCombo_->setEnabled(hasSnapshots);
    changesListWidget_->clear();
    if (!hasSnapshots) {
        snapshotComparisonLabel_->setText("No snapshots available yet. Ask a manager to create one in the Snapshots tab.");
        changesListWidget_->addItem("No historical data available.");
    } else {
        snapshotComparisonLabel_->setText("Select a snapshot to compare");
    }
}

void MyProgressWidget::updateCertificationTimeline()
{
    certificationsListWidget_->clear();

    if (certifications_.isEmpty()) {
        certificationsListWidget_->addItem("No certifications recorded yet");
        return;
    }

    // Sort by date earned (most recent first)
    QList<Certification> sortedCerts = certifications_;
    std::sort(sortedCerts.begin(), sortedCerts.end(), [](const Certification& a, const Certification& b) {
        return a.dateEarned() > b.dateEarned();
    });

    for (const Certification& cert : sortedCerts) {
        QString expiryText = cert.expiryDate().isValid()
            ? QString(" (Expires: %1)").arg(cert.expiryDate().toString("yyyy-MM-dd"))
            : " (No expiry)";

        QString certText = QString("%1 - Earned: %2%3")
            .arg(cert.name())
            .arg(cert.dateEarned().toString("yyyy-MM-dd"))
            .arg(expiryText);

        // Highlight if expiring soon (within 90 days)
        QListWidgetItem* item = new QListWidgetItem(certText);
        if (cert.expiryDate().isValid()) {
            int daysUntilExpiry = QDate::currentDate().daysTo(cert.expiryDate());
            if (daysUntilExpiry < 90 && daysUntilExpiry >= 0) {
                item->setBackground(QColor(255, 235, 59, 100));  // Yellow highlight
            } else if (daysUntilExpiry < 0) {
                item->setBackground(QColor(244, 67, 54, 100));  // Red highlight (expired)
            }
        }

        certificationsListWidget_->addItem(item);
    }
}

void MyProgressWidget::updateGoalsProgress()
{
    goalsListWidget_->clear();
    const QList<DevelopmentPlanItem> assignedItems = developmentPlanRepo_.findByEngineer(engineerId_, "active");
    for (const DevelopmentPlanItem& plan : assignedItems) {
        QString text = QString("%1 / %2\n%3\nTarget: %4 | Due: %5")
            .arg(plan.productionAreaName, plan.machineName, plan.competencyName)
            .arg(plan.targetScore)
            .arg(plan.dueDate.isValid() ? plan.dueDate.toString("dd MMM yyyy") : "Not set");
        if (!plan.managerNotes.isEmpty()) {
            text += "\nManager guidance: " + plan.managerNotes;
        } else if (!plan.guidance.isEmpty()) {
            text += "\nNext step: " + plan.guidance;
        }
        QListWidgetItem* item = new QListWidgetItem(text, goalsListWidget_);
        if (plan.dueDate.isValid() && plan.dueDate < QDate::currentDate()) {
            item->setBackground(QColor("#fee2e2"));
        } else {
            item->setBackground(QColor("#eff6ff"));
        }
    }
    if (!assignedItems.isEmpty()) {
        return;
    }

    int lowProductionCount = 0;
    for (const Assessment& assessment : assessments_) {
        if (assessment.score() <= 1) {
            lowProductionCount++;
        }
    }

    int lowCoreCount = 0;
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        if (assessment.score() <= 1) {
            lowCoreCount++;
        }
    }

    int expiredCerts = 0;
    int expiringSoonCerts = 0;
    const QDate today = QDate::currentDate();
    for (const Certification& cert : certifications_) {
        if (!cert.expiryDate().isValid()) {
            continue;
        }
        int days = today.daysTo(cert.expiryDate());
        if (days < 0) {
            expiredCerts++;
        } else if (days <= 90) {
            expiringSoonCerts++;
        }
    }

    if (lowProductionCount > 0) {
        goalsListWidget_->addItem(
            QString("Raise %1 production competencies from 0/1 to 2+").arg(lowProductionCount));
    }
    if (lowCoreCount > 0) {
        goalsListWidget_->addItem(
            QString("Raise %1 core skills from 0/1 to 2+").arg(lowCoreCount));
    }
    if (expiredCerts > 0) {
        goalsListWidget_->addItem(
            QString("Renew %1 expired certification(s)").arg(expiredCerts));
    }
    if (expiringSoonCerts > 0) {
        goalsListWidget_->addItem(
            QString("Plan renewal for %1 certification(s) expiring within 90 days").arg(expiringSoonCerts));
    }
    if (goalsListWidget_->count() == 0) {
        goalsListWidget_->addItem("Maintain current standards and keep monthly reassessments up to date.");
    }
}

void MyProgressWidget::onRefreshClicked()
{
    loadProgressData();
}

void MyProgressWidget::onSnapshotComparisonChanged(int index)
{
    changesListWidget_->clear();

    if (index <= 0) {
        snapshotComparisonLabel_->setText("Select a snapshot to compare");
        return;
    }

    // Get selected snapshot
    QString snapshotId = snapshotCombo_->itemData(index).toString();
    Snapshot selectedSnapshot;
    for (const Snapshot& snapshot : snapshots_) {
        if (snapshot.id() == snapshotId) {
            selectedSnapshot = snapshot;
            break;
        }
    }

    if (selectedSnapshot.id().isEmpty()) {
        return;
    }

    snapshotComparisonLabel_->setText(QString("Comparing current state with snapshot from %1")
        .arg(selectedSnapshot.timestamp().toString("yyyy-MM-dd HH:mm")));

    // Parse snapshot data and compare with current
    QJsonDocument doc = QJsonDocument::fromJson(selectedSnapshot.data().toUtf8());
    if (!doc.isObject()) {
        changesListWidget_->addItem("Error: Could not parse snapshot data");
        return;
    }

    QJsonObject obj = doc.object();
    QHash<int, QString> competencyNamesById;
    DataCache& cache = DataCache::instance();
    const QList<ProductionArea> areas = cache.getAreas();
    for (const ProductionArea& area : areas) {
        const QList<Machine> machines = cache.getMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            const QList<Competency> competencies = cache.getCompetenciesByMachine(machine.id());
            for (const Competency& competency : competencies) {
                competencyNamesById.insert(competency.id(), competency.name());
            }
        }
    }

    // Compare competency assessments
    int improvementsCount = 0;
    int declinesCount = 0;

    if (obj.contains("assessments")) {
        QJsonArray snapshotAssessments = obj["assessments"].toArray();

        for (const Assessment& currentAssessment : assessments_) {
            // Find matching assessment in snapshot
            for (const QJsonValue& val : snapshotAssessments) {
                QJsonObject snapshotAssessment = val.toObject();

                if (snapshotAssessment["engineerId"].toString() == engineerId_ &&
                    snapshotAssessment["competencyId"].toInt() == currentAssessment.competencyId()) {

                    int oldScore = snapshotAssessment["score"].toInt();
                    int newScore = currentAssessment.score();

                    if (newScore > oldScore) {
                        improvementsCount++;
                        const QString competencyName = competencyNamesById.value(
                            currentAssessment.competencyId(),
                            QString("Competency %1").arg(currentAssessment.competencyId()));
                        QString change = QString("✓ %1: %2 -> %3 (+%4)")
                            .arg(competencyName)
                            .arg(oldScore)
                            .arg(newScore)
                            .arg(newScore - oldScore);
                        QListWidgetItem* item = new QListWidgetItem(change);
                        item->setForeground(QColor(76, 175, 80));  // Green
                        changesListWidget_->addItem(item);
                    } else if (newScore < oldScore) {
                        declinesCount++;
                        const QString competencyName = competencyNamesById.value(
                            currentAssessment.competencyId(),
                            QString("Competency %1").arg(currentAssessment.competencyId()));
                        QString change = QString("↓ %1: %2 -> %3 (%4)")
                            .arg(competencyName)
                            .arg(oldScore)
                            .arg(newScore)
                            .arg(newScore - oldScore);
                        QListWidgetItem* item = new QListWidgetItem(change);
                        item->setForeground(QColor(244, 67, 54));  // Red
                        changesListWidget_->addItem(item);
                    }
                }
            }
        }
    }

    if (changesListWidget_->count() == 0) {
        changesListWidget_->addItem("No changes detected since this snapshot");
    } else {
        QString summary = QString("Summary: %1 improvements, %2 declines")
            .arg(improvementsCount)
            .arg(declinesCount);
        QListWidgetItem* summaryItem = new QListWidgetItem(summary);
        summaryItem->setFont(QFont("", -1, QFont::Bold));
        changesListWidget_->insertItem(0, summaryItem);
    }
}
