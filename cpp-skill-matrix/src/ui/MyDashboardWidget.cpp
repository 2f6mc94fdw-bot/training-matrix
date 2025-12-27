#include "MyDashboardWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QPolarChart>
#include <QShowEvent>

MyDashboardWidget::MyDashboardWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , isFirstShow_(true)
    , totalAssessmentsLabel_(nullptr)
    , avgSkillLevelLabel_(nullptr)
    , coreSkillsAvgLabel_(nullptr)
    , competenciesAssessedLabel_(nullptr)
    , weaknessListWidget_(nullptr)
    , productionAreaChartView_(nullptr)
    , coreSkillsChartView_(nullptr)
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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
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
    QGroupBox* statsGroup = new QGroupBox("Personal Statistics", this);
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

    // Stat card 4: Competencies Assessed
    QGroupBox* stat4 = new QGroupBox(this);
    QVBoxLayout* stat4Layout = new QVBoxLayout(stat4);
    QLabel* stat4Title = new QLabel("Competencies Assessed", this);
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
    productionAreaChartView_->setMinimumHeight(300);
    prodChartLayout->addWidget(prodChartTitle);
    prodChartLayout->addWidget(productionAreaChartView_);
    chartsLayout->addLayout(prodChartLayout);

    // Core Skills Radar Chart
    QVBoxLayout* coreChartLayout = new QVBoxLayout();
    QLabel* coreChartTitle = new QLabel("Core Skills Scores", this);
    coreChartTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    coreChartTitle->setAlignment(Qt::AlignCenter);
    coreSkillsChartView_ = new QChartView(this);
    coreSkillsChartView_->setRenderHint(QPainter::Antialiasing);
    coreSkillsChartView_->setMinimumHeight(300);
    coreChartLayout->addWidget(coreChartTitle);
    coreChartLayout->addWidget(coreSkillsChartView_);
    chartsLayout->addLayout(coreChartLayout);

    mainLayout->addWidget(chartsGroup);

    // Areas of weakness section
    QGroupBox* weaknessGroup = new QGroupBox("Areas Needing Development", this);
    QVBoxLayout* weaknessLayout = new QVBoxLayout(weaknessGroup);

    QLabel* weaknessDesc = new QLabel("Skills and competencies where you scored 0 or 1 (out of 3):", this);
    weaknessDesc->setStyleSheet("color: #666; font-style: italic;");
    weaknessLayout->addWidget(weaknessDesc);

    weaknessListWidget_ = new QListWidget(this);
    weaknessListWidget_->setMaximumHeight(150);
    weaknessLayout->addWidget(weaknessListWidget_);

    mainLayout->addWidget(weaknessGroup);
    mainLayout->addStretch();
}

void MyDashboardWidget::loadDashboardData()
{
    Logger::instance().info("MyDashboardWidget", QString("Loading dashboard for engineer: %1").arg(engineerId_));

    // Load engineer data
    currentEngineer_ = engineerRepo_.findById(engineerId_);

    // Load assessments
    QList<Assessment> allAssessments = assessmentRepo_.findAll();
    assessments_.clear();
    for (const Assessment& assessment : allAssessments) {
        if (assessment.engineerId() == engineerId_) {
            assessments_.append(assessment);
        }
    }

    // Load core skill assessments
    QList<CoreSkillAssessment> allCoreSkillAssessments = coreSkillsRepo_.findAllAssessments();
    coreSkillAssessments_.clear();
    for (const CoreSkillAssessment& assessment : allCoreSkillAssessments) {
        if (assessment.engineerId() == engineerId_) {
            coreSkillAssessments_.append(assessment);
        }
    }

    // Update all sections
    updatePersonalStats();
    updateAreasOfWeakness();
    createProductionAreaRadarChart();
    createCoreSkillsRadarChart();
}

void MyDashboardWidget::updatePersonalStats()
{
    // Total assessments (competencies + core skills)
    int totalAssessments = assessments_.size() + coreSkillAssessments_.size();
    totalAssessmentsLabel_->setText(QString::number(totalAssessments));

    // Average competency skill level
    double avgCompetency = 0.0;
    if (!assessments_.isEmpty()) {
        int totalScore = 0;
        for (const Assessment& assessment : assessments_) {
            totalScore += assessment.score();
        }
        avgCompetency = static_cast<double>(totalScore) / assessments_.size();
    }
    avgSkillLevelLabel_->setText(QString::number(avgCompetency, 'f', 2));

    // Average core skills
    double avgCoreSkills = 0.0;
    if (!coreSkillAssessments_.isEmpty()) {
        int totalScore = 0;
        for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
            totalScore += assessment.score();
        }
        avgCoreSkills = static_cast<double>(totalScore) / coreSkillAssessments_.size();
    }
    coreSkillsAvgLabel_->setText(QString::number(avgCoreSkills, 'f', 2));

    // Competencies assessed (unique competencies)
    competenciesAssessedLabel_->setText(QString::number(assessments_.size()));
}

void MyDashboardWidget::updateAreasOfWeakness()
{
    weaknessListWidget_->clear();

    // Find competencies with score 0 or 1
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Build list of all machines and competencies
    QList<Machine> allMachines;
    QList<Competency> allCompetencies;
    for (const ProductionArea& area : areas) {
        QList<Machine> areaMachines = productionRepo_.findMachinesByArea(area.id());
        allMachines.append(areaMachines);

        for (const Machine& machine : areaMachines) {
            QList<Competency> machineCompetencies = productionRepo_.findCompetenciesByMachine(machine.id());
            allCompetencies.append(machineCompetencies);
        }
    }

    for (const Assessment& assessment : assessments_) {
        if (assessment.score() <= 1) {
            // Find the competency name
            QString competencyName;
            int machineId = assessment.machineId();
            int competencyId = assessment.competencyId();

            for (const Competency& comp : allCompetencies) {
                if (comp.id() == competencyId) {
                    competencyName = comp.name();
                    break;
                }
            }

            // Find machine and area names
            QString machineName;
            int areaId = assessment.productionAreaId();
            for (const Machine& machine : allMachines) {
                if (machine.id() == machineId) {
                    machineName = machine.name();
                    break;
                }
            }

            QString areaName;
            for (const ProductionArea& area : areas) {
                if (area.id() == areaId) {
                    areaName = area.name();
                    break;
                }
            }

            QString weaknessText = QString("%1 - %2 - %3 (Score: %4)")
                .arg(areaName)
                .arg(machineName)
                .arg(competencyName)
                .arg(assessment.score());
            weaknessListWidget_->addItem(weaknessText);
        }
    }

    // Find core skills with score 0 or 1
    QList<CoreSkill> allCoreSkills = coreSkillsRepo_.findAllSkills();
    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();

    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        if (assessment.score() <= 1) {
            QString skillName;
            QString categoryName;

            for (const CoreSkill& skill : allCoreSkills) {
                if (skill.id() == assessment.skillId()) {
                    skillName = skill.name();
                    break;
                }
            }

            for (const CoreSkillCategory& category : categories) {
                if (category.id() == assessment.categoryId()) {
                    categoryName = category.name();
                    break;
                }
            }

            QString weaknessText = QString("Core Skill: %1 - %2 (Score: %3)")
                .arg(categoryName)
                .arg(skillName)
                .arg(assessment.score());
            weaknessListWidget_->addItem(weaknessText);
        }
    }

    if (weaknessListWidget_->count() == 0) {
        weaknessListWidget_->addItem("No areas needing development - Great job!");
    }
}

void MyDashboardWidget::createProductionAreaRadarChart()
{
    // Calculate average score per production area
    QMap<QString, double> areaScores;
    QMap<QString, int> areaCounts;

    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Initialize maps
    for (const ProductionArea& area : areas) {
        areaScores[area.name()] = 0.0;
        areaCounts[area.name()] = 0;
    }

    // Calculate averages
    for (const Assessment& assessment : assessments_) {
        int areaId = assessment.productionAreaId();
        for (const ProductionArea& area : areas) {
            if (area.id() == areaId) {
                areaScores[area.name()] += assessment.score();
                areaCounts[area.name()]++;
                break;
            }
        }
    }

    // Compute averages
    QMap<QString, double> averages;
    for (const QString& areaName : areaScores.keys()) {
        if (areaCounts[areaName] > 0) {
            averages[areaName] = areaScores[areaName] / areaCounts[areaName];
        } else {
            averages[areaName] = 0.0;
        }
    }

    QChart* chart = createRadarChart(averages, "Production Areas", QColor("#2196F3"));
    productionAreaChartView_->setChart(chart);
}

void MyDashboardWidget::createCoreSkillsRadarChart()
{
    // Calculate average score per core skill category
    QMap<QString, double> categoryScores;
    QMap<QString, int> categoryCounts;

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();

    // Initialize maps
    for (const CoreSkillCategory& category : categories) {
        categoryScores[category.name()] = 0.0;
        categoryCounts[category.name()] = 0;
    }

    // Calculate averages
    for (const CoreSkillAssessment& assessment : coreSkillAssessments_) {
        QString categoryId = assessment.categoryId();
        for (const CoreSkillCategory& category : categories) {
            if (category.id() == categoryId) {
                categoryScores[category.name()] += assessment.score();
                categoryCounts[category.name()]++;
                break;
            }
        }
    }

    // Compute averages
    QMap<QString, double> averages;
    for (const QString& categoryName : categoryScores.keys()) {
        if (categoryCounts[categoryName] > 0) {
            averages[categoryName] = categoryScores[categoryName] / categoryCounts[categoryName];
        } else {
            averages[categoryName] = 0.0;
        }
    }

    QChart* chart = createRadarChart(averages, "Core Skills", QColor("#FF9800"));
    coreSkillsChartView_->setChart(chart);
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
