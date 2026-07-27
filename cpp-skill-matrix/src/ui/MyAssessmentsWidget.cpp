#include "MyAssessmentsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include "../core/DataCache.h"
#include "../core/Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

MyAssessmentsWidget::MyAssessmentsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , assessmentsLayout_(nullptr)
    , assessmentsContainer_(nullptr)
    , saveButton_(nullptr)
    , refreshButton_(nullptr)
    , markNotificationsReadButton_(nullptr)
    , summaryLabel_(nullptr)
    , notificationsSummaryLabel_(nullptr)
    , notificationsContainer_(nullptr)
    , notificationsLayout_(nullptr)
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
    QLabel* descLabel = new QLabel(
        "Propose changes to your machine competency scores. Changes become official after manager approval.", this);
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

    // Manager notifications panel
    QGroupBox* notificationsBox = new QGroupBox("Manager Updates", this);
    QVBoxLayout* notificationsBoxLayout = new QVBoxLayout(notificationsBox);
    notificationsBoxLayout->setSpacing(8);

    notificationsSummaryLabel_ = new QLabel("No manager updates", this);
    notificationsSummaryLabel_->setStyleSheet("QLabel { color: #475569; font-size: 12px; }");
    notificationsBoxLayout->addWidget(notificationsSummaryLabel_);

    notificationsContainer_ = new QWidget(this);
    notificationsLayout_ = new QVBoxLayout(notificationsContainer_);
    notificationsLayout_->setSpacing(6);
    notificationsLayout_->setContentsMargins(0, 0, 0, 0);
    notificationsBoxLayout->addWidget(notificationsContainer_);

    markNotificationsReadButton_ = new QPushButton("Mark Updates as Read", this);
    markNotificationsReadButton_->setMinimumWidth(170);
    connect(markNotificationsReadButton_, &QPushButton::clicked, this, &MyAssessmentsWidget::onMarkNotificationsReadClicked);
    notificationsBoxLayout->addWidget(markNotificationsReadButton_, 0, Qt::AlignLeft);

    mainLayout->addWidget(notificationsBox);

    // Skill level legend
    QLabel* legendLabel = new QLabel(
        "Competency Levels: 0 = No knowledge | 1 = Limited (less than 3 occurrences) | "
        "2 = Trained (greater than 3 occurrences) | 3 = Able to train others competently", this);
    legendLabel->setStyleSheet("QLabel { color: #666; font-size: 11pt; }");
    legendLabel->setWordWrap(true);
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

    saveButton_ = new QPushButton("Submit Changes for Approval", this);
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

    // Clear manager notifications UI
    QLayoutItem* noteItem;
    while ((noteItem = notificationsLayout_->takeAt(0)) != nullptr) {
        if (noteItem->widget()) {
            noteItem->widget()->deleteLater();
        }
        delete noteItem;
    }

    // Load production data
    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    // Load this engineer's assessments
    QList<Assessment> assessments = assessmentRepo_.findByEngineer(engineerId_);

    // Create a map of assessment key -> score for quick lookup
    QMap<QString, int> assessmentScores;
    for (const Assessment& assessment : assessments) {
        QString key = QString("%1_%2_%3")
            .arg(assessment.productionAreaId())
            .arg(assessment.machineId())
            .arg(assessment.competencyId());
        assessmentScores[key] = assessment.score();
    }

    QMap<QString, int> pendingScores;
    const QList<AssessmentSubmission> pendingSubmissions =
        submissionRepo_.findPendingByEngineer(engineerId_, "production");
    for (const AssessmentSubmission& submission : pendingSubmissions) {
        const QString key = QString("%1_%2_%3")
            .arg(submission.productionAreaId)
            .arg(submission.machineId)
            .arg(submission.competencyId);
        pendingScores[key] = submission.proposedScore;
    }

    // Track statistics
    int totalCompetencies = 0;
    int trainedCompetencies = 0;

    // Create a card for each production area
    DataCache& cache = DataCache::instance();
    if (!cache.isLoaded()) {
        cache.load();
    }
    for (const ProductionArea& area : areas) {
        // Get machines for this area
        QList<Machine> areaMachines = cache.getMachinesByArea(area.id());

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
            QList<Competency> competencies = cache.getCompetenciesByMachine(machine.id());

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

                compLayout->addSpacing(16);

                // Get current score
                QString key = QString("%1_%2_%3")
                    .arg(area.id())
                    .arg(machine.id())
                    .arg(competency.id());
                const int approvedScore = assessmentScores.value(key, 0);
                const bool pendingApproval = pendingScores.contains(key);
                const int currentScore = pendingApproval
                    ? pendingScores.value(key)
                    : approvedScore;

                if (approvedScore > 0) {
                    trainedCompetencies++;
                }

                // Create score buttons (0-3)
                createScoreButtons(compLayout, area.id(), machine.id(), competency.id(), currentScore, pendingApproval);

                cardLayout->addLayout(compLayout);
            }
        }

        assessmentsLayout_->addWidget(areaCard);
    }

    // Add stretch at the end
    assessmentsLayout_->addStretch();

    // Update summary
    double completionRate = totalCompetencies > 0 ? (double)trainedCompetencies / totalCompetencies * 100.0 : 0.0;

    QString summaryText = QString("Approved: %1 of %2 competencies trained (%3%) | Pending changes: %4")
        .arg(trainedCompetencies)
        .arg(totalCompetencies)
        .arg(completionRate, 0, 'f', 1)
        .arg(pendingSubmissions.size());

    summaryLabel_->setText(summaryText);

    // Load notifications about manager adjustments
    const QList<EngineerNotification> notifications = notificationRepo_.findByEngineer(engineerId_, true, 20);
    if (notifications.isEmpty()) {
        notificationsSummaryLabel_->setText("No unread manager updates.");
        markNotificationsReadButton_->setEnabled(false);
    } else {
        notificationsSummaryLabel_->setText(
            QString("%1 unread manager update%2").arg(notifications.size()).arg(notifications.size() == 1 ? "" : "s"));
        markNotificationsReadButton_->setEnabled(true);

        for (const EngineerNotification& note : notifications) {
            QLabel* noteLabel = new QLabel(
                QString("%1  |  %2\n%3")
                    .arg(note.createdAt.toString("dd MMM yyyy HH:mm"))
                    .arg(note.title)
                    .arg(note.message),
                this);
            noteLabel->setWordWrap(true);
            noteLabel->setStyleSheet(
                "QLabel {"
                "  border: 1px solid #cbd5e1;"
                "  border-radius: 8px;"
                "  padding: 8px;"
                "  background-color: #f8fafc;"
                "}");
            notificationsLayout_->addWidget(noteLabel);
        }
    }

    Logger::instance().info("MyAssessmentsWidget",
        QString("Loaded %1 competencies (%2 trained) for engineer %3")
        .arg(totalCompetencies)
        .arg(trainedCompetencies)
        .arg(engineerId_));
}

void MyAssessmentsWidget::createScoreButtons(QHBoxLayout* layout, int areaId, int machineId,
                                             int competencyId, int currentScore, bool pendingApproval)
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
        button->setFixedSize(60, 48);
        button->setCursor(Qt::PointingHandCursor);
        auto* shadow = new QGraphicsDropShadowEffect(button);
        shadow->setBlurRadius(12);
        shadow->setOffset(0, 2);
        shadow->setColor(QColor(15, 23, 42, 40));
        button->setGraphicsEffect(shadow);

        // Store metadata
        button->setProperty("areaId", areaId);
        button->setProperty("machineId", machineId);
        button->setProperty("competencyId", competencyId);
        button->setProperty("score", score);

        // Style button based on whether it's selected
        bool isSelected = (score == currentScore);
        button->setProperty("isSelected", isSelected);  // Store for save detection

        QString buttonStyle;
        if (isSelected) {
            // Active button: colored background, white text
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: %1;"
                "    color: white;"
                "    border: 2px solid %1;"
                "    border-radius: 14px;"
                "    padding: 0px;"
                "    min-width: 0px;"
                "    min-height: 0px;"
                "    text-align: center;"
                "    font-weight: bold;"
                "    font-size: 20px;"
                "}"
                "QPushButton:hover {"
                "    border: 2px solid #1e293b;"
                "    background-color: %1;"
                "}"
            ).arg(scoreInfos[score].color);
        } else {
            // Inactive button: transparent background, colored border
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: #f8fafc;"
                "    color: #475569;"
                "    border: 2px solid #cbd5e1;"
                "    border-radius: 14px;"
                "    padding: 0px;"
                "    min-width: 0px;"
                "    min-height: 0px;"
                "    text-align: center;"
                "    font-size: 20px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    border-color: %1;"
                "    color: %1;"
                "    background-color: #e2e8f0;"
                "}"
            ).arg(scoreInfos[score].color);
        }

        button->setStyleSheet(buttonStyle);

        connect(button, &QPushButton::clicked, this, &MyAssessmentsWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    if (pendingApproval) {
        QLabel* pendingLabel = new QLabel("Pending approval", this);
        pendingLabel->setStyleSheet(
            "QLabel { color: #b45309; background: #fffbeb; border: 1px solid #fbbf24; "
            "border-radius: 8px; padding: 6px 10px; font-weight: 600; }");
        layout->addWidget(pendingLabel);
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

                // Store selected state as property for reliable save detection
                button->setProperty("isSelected", isSelected);

                QString buttonStyle;
                if (isSelected) {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: %1;"
                        "    color: white;"
                        "    border: 2px solid %1;"
                        "    border-radius: 14px;"
                        "    padding: 0px;"
                        "    min-width: 0px;"
                        "    min-height: 0px;"
                        "    text-align: center;"
                        "    font-weight: bold;"
                        "    font-size: 20px;"
                        "}"
                        "QPushButton:hover {"
                        "    border: 2px solid #1e293b;"
                        "    background-color: %1;"
                        "}"
                    ).arg(scoreColors[i]);
                } else {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: #f8fafc;"
                        "    color: #475569;"
                        "    border: 2px solid #cbd5e1;"
                        "    border-radius: 14px;"
                        "    padding: 0px;"
                        "    min-width: 0px;"
                        "    min-height: 0px;"
                        "    text-align: center;"
                        "    font-size: 20px;"
                        "    font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "    border-color: %1;"
                        "    color: %1;"
                        "    background-color: #e2e8f0;"
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
    int submittedCount = 0;
    int errorCount = 0;

    QList<Assessment> existingAssessments = assessmentRepo_.findByEngineer(engineerId_);
    QMap<QString, int> existingScores;
    for (const Assessment& assessment : existingAssessments) {
        const QString key = QString("%1_%2_%3")
            .arg(assessment.productionAreaId())
            .arg(assessment.machineId())
            .arg(assessment.competencyId());
        existingScores[key] = assessment.score();
    }

    QMap<QString, int> pendingScores;
    const QList<AssessmentSubmission> pendingSubmissions =
        submissionRepo_.findPendingByEngineer(engineerId_, "production");
    for (const AssessmentSubmission& submission : pendingSubmissions) {
        const QString key = QString("%1_%2_%3")
            .arg(submission.productionAreaId)
            .arg(submission.machineId)
            .arg(submission.competencyId);
        pendingScores[key] = submission.proposedScore;
    }

    Session* session = Application::instance().session();
    const QString submittedByUserId = session ? session->userId() : QString();
    const QString submittedByName = session ? session->username() : QString();

    // Loop through all button groups and find selected score for each competency
    for (const ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        // Find which button is selected using the isSelected property
        int selectedScore = 0;
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            if (button->property("isSelected").toBool()) {
                selectedScore = score;
                break;
            }
        }

        const QString scoreKey = QString("%1_%2_%3")
            .arg(buttonGroup.areaId)
            .arg(buttonGroup.machineId)
            .arg(buttonGroup.competencyId);
        const int approvedScore = existingScores.value(scoreKey, 0);
        const int displayedScore = pendingScores.contains(scoreKey)
            ? pendingScores.value(scoreKey)
            : approvedScore;

        if (selectedScore == displayedScore) {
            continue;
        }

        if (submissionRepo_.submitProduction(
                engineerId_, buttonGroup.areaId, buttonGroup.machineId,
                buttonGroup.competencyId, selectedScore,
                submittedByUserId, submittedByName)) {
            submittedCount++;
        } else {
            errorCount++;
            Logger::instance().error("MyAssessmentsWidget",
                QString("Failed to submit assessment: %1").arg(submissionRepo_.lastError()));
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Submitted %1 changes, but %2 failed.").arg(submittedCount).arg(errorCount));
    } else if (submittedCount == 0) {
        QMessageBox::information(this, "No Changes", "There are no new assessment changes to submit.");
    } else {
        Logger::instance().info("MyAssessmentsWidget", QString("Submitted %1 assessment changes").arg(submittedCount));
        QMessageBox::information(this, "Submitted for Approval",
            QString("Submitted %1 assessment change%2 for manager approval.")
                .arg(submittedCount)
                .arg(submittedCount == 1 ? "" : "s"));
    }
    loadAssessments();
}

void MyAssessmentsWidget::onRefreshClicked()
{
    loadAssessments();
    Logger::instance().info("MyAssessmentsWidget", "Assessments refreshed");
}

void MyAssessmentsWidget::onMarkNotificationsReadClicked()
{
    if (!notificationRepo_.markAllReadForEngineer(engineerId_)) {
        QMessageBox::warning(this, "Update Failed",
            "Could not mark manager updates as read: " + notificationRepo_.lastError());
        return;
    }
    loadAssessments();
}
