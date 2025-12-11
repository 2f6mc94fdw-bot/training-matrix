#include "DashboardWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QDateTime>

DashboardWidget::DashboardWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCountLabel_(nullptr)
    , productionAreaCountLabel_(nullptr)
    , assessmentCountLabel_(nullptr)
    , lastUpdateLabel_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadStatistics();
    Logger::instance().info("DashboardWidget", "Dashboard widget initialized");
}

DashboardWidget::~DashboardWidget()
{
}

void DashboardWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel("Dashboard Overview", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Welcome message
    QLabel* welcomeLabel = new QLabel("Welcome to the Skill Matrix Management System", this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(12);
    welcomeLabel->setFont(welcomeFont);
    mainLayout->addWidget(welcomeLabel);

    mainLayout->addSpacing(20);

    // Statistics Grid
    QGridLayout* statsGrid = new QGridLayout();

    // Engineers Card
    QGroupBox* engineersBox = new QGroupBox("Total Engineers", this);
    QVBoxLayout* engineersLayout = new QVBoxLayout(engineersBox);
    engineerCountLabel_ = new QLabel("0", this);
    QFont countFont = engineerCountLabel_->font();
    countFont.setPointSize(32);
    countFont.setBold(true);
    engineerCountLabel_->setFont(countFont);
    engineerCountLabel_->setAlignment(Qt::AlignCenter);
    engineersLayout->addWidget(engineerCountLabel_);
    engineersBox->setMinimumHeight(120);

    // Production Areas Card
    QGroupBox* areasBox = new QGroupBox("Production Areas", this);
    QVBoxLayout* areasLayout = new QVBoxLayout(areasBox);
    productionAreaCountLabel_ = new QLabel("0", this);
    productionAreaCountLabel_->setFont(countFont);
    productionAreaCountLabel_->setAlignment(Qt::AlignCenter);
    areasLayout->addWidget(productionAreaCountLabel_);
    areasBox->setMinimumHeight(120);

    // Assessments Card
    QGroupBox* assessmentsBox = new QGroupBox("Total Assessments", this);
    QVBoxLayout* assessmentsLayout = new QVBoxLayout(assessmentsBox);
    assessmentCountLabel_ = new QLabel("0", this);
    assessmentCountLabel_->setFont(countFont);
    assessmentCountLabel_->setAlignment(Qt::AlignCenter);
    assessmentsLayout->addWidget(assessmentCountLabel_);
    assessmentsBox->setMinimumHeight(120);

    statsGrid->addWidget(engineersBox, 0, 0);
    statsGrid->addWidget(areasBox, 0, 1);
    statsGrid->addWidget(assessmentsBox, 0, 2);

    mainLayout->addLayout(statsGrid);

    mainLayout->addSpacing(20);

    // Quick Actions
    QGroupBox* actionsBox = new QGroupBox("Quick Actions", this);
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsBox);

    QLabel* actionsLabel = new QLabel(
        "Use the navigation menu on the left to:\n"
        "  - Manage engineers and their information\n"
        "  - Define production areas, machines, and competencies\n"
        "  - Perform competency assessments\n"
        "  - View reports and analytics\n"
        "  - Export data for external use", this);

    actionsLayout->addWidget(actionsLabel);
    mainLayout->addWidget(actionsBox);

    mainLayout->addSpacing(20);

    // Last Update
    lastUpdateLabel_ = new QLabel("Last updated: Never", this);
    mainLayout->addWidget(lastUpdateLabel_);

    // Refresh Button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    refreshButton_ = new QPushButton("Refresh Statistics", this);
    connect(refreshButton_, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
    buttonLayout->addWidget(refreshButton_);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void DashboardWidget::loadStatistics()
{
    int engineerCount = engineerRepo_.findAll().size();
    int areaCount = productionRepo_.findAllAreas().size();
    int assessmentCount = assessmentRepo_.findAll().size();

    engineerCountLabel_->setText(QString::number(engineerCount));
    productionAreaCountLabel_->setText(QString::number(areaCount));
    assessmentCountLabel_->setText(QString::number(assessmentCount));

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    lastUpdateLabel_->setText("Last updated: " + timestamp);

    Logger::instance().info("DashboardWidget",
        QString("Statistics loaded: %1 engineers, %2 areas, %3 assessments")
        .arg(engineerCount).arg(areaCount).arg(assessmentCount));
}

void DashboardWidget::onRefreshClicked()
{
    loadStatistics();
}
