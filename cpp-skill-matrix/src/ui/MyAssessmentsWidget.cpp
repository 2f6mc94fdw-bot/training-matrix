#include "MyAssessmentsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>
#include <QMessageBox>

MyAssessmentsWidget::MyAssessmentsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , assessmentsLayout_(nullptr)
    , assessmentsContainer_(nullptr)
    , saveButton_(nullptr)
    , refreshButton_(nullptr)
    , summaryLabel_(nullptr)
{
    setupUI();
    loadAssessments();
    Logger::instance().info("MyAssessmentsWidget", QString("My Assessments widget initialized for engineer: %1").arg(engineerId_));
}

MyAssessmentsWidget::~MyAssessmentsWidget()
{
}

void MyAssessmentsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel* titleLabel = new QLabel("My Assessments", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Description
    QLabel* descLabel = new QLabel("View and update your machine competency assessments.", this);
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    // Summary
    summaryLabel_ = new QLabel("Loading...", this);
    QFont summaryFont = summaryLabel_->font();
    summaryFont.setPointSize(12);
    summaryFont.setBold(true);
    summaryLabel_->setFont(summaryFont);
    summaryLabel_->setStyleSheet("QLabel { color: " + QString(Constants::BRAND_ACCENT) + "; padding: 10px; }");
    mainLayout->addWidget(summaryLabel_);

    // Skill level legend
    QLabel* legendLabel = new QLabel("Competency Levels: 0 = Not Trained | 1 = Basic | 2 = Competent | 3 = Expert", this);
    legendLabel->setStyleSheet("QLabel { color: #666; font-size: 11pt; }");
    mainLayout->addWidget(legendLabel);

    // Scrollable assessments container
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    assessmentsContainer_ = new QWidget();
    assessmentsLayout_ = new QVBoxLayout(assessmentsContainer_);
    assessmentsLayout_->setSpacing(16);
    assessmentsLayout_->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(assessmentsContainer_);
    mainLayout->addWidget(scrollArea);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    saveButton_ = new QPushButton("Save My Assessments", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(saveButton_, &QPushButton::clicked, this, &MyAssessmentsWidget::onSaveClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &MyAssessmentsWidget::onRefreshClicked);

    buttonLayout->addWidget(saveButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void MyAssessmentsWidget::loadAssessments()
{
    // Clear existing widgets
    QLayoutItem* item;
    while ((item = assessmentsLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    scoreButtonGroups_.clear();

    // Load production data
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Load this engineer's assessments
    QList<Assessment> assessments = assessmentRepo_.findAll();

    // Create a map of assessment key -> score for quick lookup
    QMap<QString, int> assessmentScores;
    for (const Assessment& assessment : assessments) {
        if (assessment.engineerId() == engineerId_) {
            QString key = QString("%1_%2_%3")
                .arg(assessment.productionAreaId())
                .arg(assessment.machineId())
                .arg(assessment.competencyId());
            assessmentScores[key] = assessment.score();
        }
    }

    // Track statistics
    int totalCompetencies = 0;
    int trainedCompetencies = 0;

    // Create a card for each production area
    for (const ProductionArea& area : areas) {
        // Get machines for this area
        QList<Machine> areaMachines = productionRepo_.findMachinesByArea(area.id());

        if (areaMachines.isEmpty()) {
            continue;
        }

        // Create area card
        QGroupBox* areaCard = new QGroupBox(this);
        areaCard->setStyleSheet(
            "QGroupBox {"
            "    border: 2px solid #e2e8f0;"
            "    border-radius: 8px;"
            "    padding: 16px;"
            "    background-color: transparent;"
            "    margin-top: 12px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 16px;"
            "    padding: 0 8px 0 8px;"
            "}"
        );

        QVBoxLayout* cardLayout = new QVBoxLayout(areaCard);
        cardLayout->setSpacing(12);

        // Area title
        QLabel* areaLabel = new QLabel(area.name(), this);
        QFont areaFont = areaLabel->font();
        areaFont.setPointSize(14);
        areaFont.setBold(true);
        areaLabel->setFont(areaFont);
        cardLayout->addWidget(areaLabel);

        // Add machines and competencies for this area
        for (const Machine& machine : areaMachines) {
            // Machine header
            QLabel* machineLabel = new QLabel(machine.name(), this);
            QFont machineFont = machineLabel->font();
            machineFont.setPointSize(12);
            machineFont.setBold(true);
            machineLabel->setFont(machineFont);
            machineLabel->setStyleSheet("QLabel { color: #4a5568; margin-top: 8px; }");
            cardLayout->addWidget(machineLabel);

            // Get competencies for this machine
            QList<Competency> competencies = productionRepo_.findCompetenciesByMachine(machine.id());

            for (const Competency& competency : competencies) {
                totalCompetencies++;

                QHBoxLayout* compLayout = new QHBoxLayout();
                compLayout->setSpacing(12);
                compLayout->setContentsMargins(32, 4, 0, 4);

                // Competency name
                QLabel* compLabel = new QLabel(competency.name(), this);
                QFont compFont = compLabel->font();
                compFont.setPointSize(13);
                compLabel->setFont(compFont);
                compLabel->setWordWrap(true);
                compLabel->setMinimumWidth(250);
                compLabel->setMaximumWidth(500);
                compLayout->addWidget(compLabel, 1);

                compLayout->addStretch();

                // Get current score
                QString key = QString("%1_%2_%3")
                    .arg(area.id())
                    .arg(machine.id())
                    .arg(competency.id());
                int currentScore = assessmentScores.value(key, 0);

                if (currentScore > 0) {
                    trainedCompetencies++;
                }

                // Create score buttons (0-3)
                createScoreButtons(compLayout, area.id(), machine.id(), competency.id(), currentScore);

                cardLayout->addLayout(compLayout);
            }
        }

        assessmentsLayout_->addWidget(areaCard);
    }

    // Add stretch at the end
    assessmentsLayout_->addStretch();

    // Update summary
    double completionRate = totalCompetencies > 0 ? (double)trainedCompetencies / totalCompetencies * 100.0 : 0.0;

    QString summaryText = QString("📊 %1 of %2 competencies trained (%.1%%)")
        .arg(trainedCompetencies)
        .arg(totalCompetencies)
        .arg(completionRate, 0, 'f', 1);

    summaryLabel_->setText(summaryText);

    Logger::instance().info("MyAssessmentsWidget",
        QString("Loaded %1 competencies (%2 trained) for engineer %3")
        .arg(totalCompetencies)
        .arg(trainedCompetencies)
        .arg(engineerId_));
}

void MyAssessmentsWidget::createScoreButtons(QHBoxLayout* layout, int areaId, int machineId,
                                             int competencyId, int currentScore)
{
    // Score labels and colors
    struct ScoreInfo {
        QString label;
        QString color;
    };

    QList<ScoreInfo> scoreInfos = {
        {"0", "#ff6b6b"},  // Not Trained - Red
        {"1", "#fbbf24"},  // Basic - Yellow
        {"2", "#60a5fa"},  // Competent - Blue
        {"3", "#4ade80"}   // Expert - Green
    };

    ScoreButtonGroup buttonGroup;
    buttonGroup.areaId = areaId;
    buttonGroup.machineId = machineId;
    buttonGroup.competencyId = competencyId;

    for (int score = 0; score < 4; score++) {
        QPushButton* button = new QPushButton(scoreInfos[score].label);
        button->setFixedSize(32, 32);
        button->setCursor(Qt::PointingHandCursor);

        // Store metadata
        button->setProperty("areaId", areaId);
        button->setProperty("machineId", machineId);
        button->setProperty("competencyId", competencyId);
        button->setProperty("score", score);

        // Style button based on whether it's selected
        bool isSelected = (score == currentScore);

        QString buttonStyle;
        if (isSelected) {
            // Active button: colored background, white text
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: %1;"
                "    color: white;"
                "    border: 2px solid %1;"
                "    border-radius: 16px;"
                "    font-weight: bold;"
                "    font-size: 12px;"
                "}"
                "QPushButton:hover {"
                "    opacity: 0.9;"
                "}"
            ).arg(scoreInfos[score].color);
        } else {
            // Inactive button: transparent background, colored border
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: transparent;"
                "    color: #64748b;"
                "    border: 2px solid #e2e8f0;"
                "    border-radius: 16px;"
                "    font-size: 12px;"
                "}"
                "QPushButton:hover {"
                "    border-color: %1;"
                "    color: %1;"
                "    background-color: rgba(255, 255, 255, 0.05);"
                "}"
            ).arg(scoreInfos[score].color);
        }

        button->setStyleSheet(buttonStyle);

        connect(button, &QPushButton::clicked, this, &MyAssessmentsWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    scoreButtonGroups_.append(buttonGroup);
}

void MyAssessmentsWidget::onScoreButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) {
        return;
    }

    int areaId = clickedButton->property("areaId").toInt();
    int machineId = clickedButton->property("machineId").toInt();
    int competencyId = clickedButton->property("competencyId").toInt();
    int score = clickedButton->property("score").toInt();

    // Color scheme
    QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};

    // Find the button group and update all buttons in that group
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.areaId == areaId &&
            buttonGroup.machineId == machineId &&
            buttonGroup.competencyId == competencyId) {

            // Update styles for all buttons in this group
            for (int i = 0; i < 4; i++) {
                QPushButton* button = buttonGroup.buttons[i];
                bool isSelected = (i == score);

                QString buttonStyle;
                if (isSelected) {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: %1;"
                        "    color: white;"
                        "    border: 2px solid %1;"
                        "    border-radius: 16px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    opacity: 0.9;"
                        "}"
                    ).arg(scoreColors[i]);
                } else {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: transparent;"
                        "    color: #64748b;"
                        "    border: 2px solid #e2e8f0;"
                        "    border-radius: 16px;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    border-color: %1;"
                        "    color: %1;"
                        "    background-color: rgba(255, 255, 255, 0.05);"
                        "}"
                    ).arg(scoreColors[i]);
                }

                button->setStyleSheet(buttonStyle);
            }

            // Log the score change
            Logger::instance().info("MyAssessmentsWidget",
                QString("Score changed to %1 for competency %2")
                    .arg(score)
                    .arg(competencyId));

            break;
        }
    }
}

void MyAssessmentsWidget::onSaveClicked()
{
    int savedCount = 0;
    int errorCount = 0;

    // Loop through all button groups and find selected score for each competency
    for (const ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        // Find which button is selected (has the score property and colored background)
        int selectedScore = 0;
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            // Check if button has a colored background (contains "background-color: #" in style)
            if (button->styleSheet().contains("background-color: #")) {
                selectedScore = score;
                break;
            }
        }

        Assessment assessment(0, engineerId_, buttonGroup.areaId, buttonGroup.machineId,
                            buttonGroup.competencyId, selectedScore);

        if (assessmentRepo_.saveOrUpdate(assessment)) {
            savedCount++;
        } else {
            errorCount++;
            Logger::instance().error("MyAssessmentsWidget",
                QString("Failed to save assessment: %1").arg(assessmentRepo_.lastError()));
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Saved %1 assessments, but %2 failed.").arg(savedCount).arg(errorCount));
    } else {
        Logger::instance().info("MyAssessmentsWidget", QString("Saved %1 assessments").arg(savedCount));
        QMessageBox::information(this, "Success",
            QString("Successfully saved %1 assessments.").arg(savedCount));

        // Refresh to update summary statistics
        loadAssessments();
    }
}

void MyAssessmentsWidget::onRefreshClicked()
{
    loadAssessments();
    Logger::instance().info("MyAssessmentsWidget", "Assessments refreshed");
}
