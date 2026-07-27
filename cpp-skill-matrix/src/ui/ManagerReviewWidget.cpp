#include "ManagerReviewWidget.h"

#include "../core/Application.h"
#include "../utils/Logger.h"

#include <QElapsedTimer>
#include <QColor>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QString displayType(const QString& type)
{
    return type == "core" ? "Core Skill" : "Production";
}

QColor decisionColour(const QString& decision)
{
    if (decision == "approved") {
        return QColor("#dcfce7");
    }
    if (decision == "rejected") {
        return QColor("#fee2e2");
    }
    return QColor("#fef3c7");
}
}

ManagerReviewWidget::ManagerReviewWidget(QWidget* parent)
    : QWidget(parent)
    , typeFilterCombo_(nullptr)
    , areaFilterCombo_(nullptr)
    , focusFilterCombo_(nullptr)
    , searchEdit_(nullptr)
    , summaryLabel_(nullptr)
    , table_(nullptr)
    , decisionNoteEdit_(nullptr)
    , selectedItemLabel_(nullptr)
    , approveButton_(nullptr)
    , rejectButton_(nullptr)
{
    setupUI();
    loadData();
}

ManagerReviewWidget::~ManagerReviewWidget() = default;

void ManagerReviewWidget::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    auto* titleLabel = new QLabel("Assessment Approval Queue", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    auto* subtitleLabel = new QLabel(
        "Review engineer proposals before they become official production or core-skill scores.", this);
    subtitleLabel->setStyleSheet("color: #64748b; font-size: 13px;");
    mainLayout->addWidget(subtitleLabel);

    auto* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);

    typeFilterCombo_ = new QComboBox(this);
    typeFilterCombo_->addItem("All Assessment Types", "all");
    typeFilterCombo_->addItem("Production", "production");
    typeFilterCombo_->addItem("Core Skills", "core");
    typeFilterCombo_->setMinimumWidth(190);
    filterLayout->addWidget(typeFilterCombo_);

    areaFilterCombo_ = new QComboBox(this);
    areaFilterCombo_->addItem("All Areas", "all");
    areaFilterCombo_->setMinimumWidth(210);
    filterLayout->addWidget(areaFilterCombo_);

    focusFilterCombo_ = new QComboBox(this);
    focusFilterCombo_->addItem("Pending Approval", "pending");
    focusFilterCombo_->addItem("Approved History", "approved");
    focusFilterCombo_->addItem("Rejected History", "rejected");
    focusFilterCombo_->addItem("All Submissions", "all");
    focusFilterCombo_->setMinimumWidth(190);
    filterLayout->addWidget(focusFilterCombo_);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Search engineer, machine, skill...");
    searchEdit_->setMinimumWidth(280);
    filterLayout->addWidget(searchEdit_, 1);

    auto* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setMinimumWidth(110);
    filterLayout->addWidget(refreshButton);
    mainLayout->addLayout(filterLayout);

    summaryLabel_ = new QLabel(this);
    summaryLabel_->setStyleSheet(
        "QLabel { color: #334155; background: #f8fafc; border: 1px solid #cbd5e1; "
        "border-radius: 10px; padding: 10px 14px; font-weight: 600; }");
    mainLayout->addWidget(summaryLabel_);

    table_ = new QTableWidget(this);
    table_->setColumnCount(11);
    table_->setHorizontalHeaderLabels({
        "Engineer", "Shift", "Type", "Area", "Machine / Discipline", "Competency",
        "Approved", "Proposed", "Status", "Submitted", "Reviewed By"
    });
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setSortingEnabled(false);
    table_->verticalHeader()->setVisible(false);
    for (int column = 0; column < table_->columnCount(); ++column) {
        table_->horizontalHeader()->setSectionResizeMode(column, QHeaderView::ResizeToContents);
    }
    table_->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    mainLayout->addWidget(table_, 1);

    auto* actionGroup = new QGroupBox("Decision", this);
    auto* actionLayout = new QVBoxLayout(actionGroup);
    actionLayout->setSpacing(10);

    selectedItemLabel_ = new QLabel("Select a pending submission to review.", this);
    selectedItemLabel_->setStyleSheet("font-weight: 600; color: #334155;");
    actionLayout->addWidget(selectedItemLabel_);

    decisionNoteEdit_ = new QPlainTextEdit(this);
    decisionNoteEdit_->setPlaceholderText(
        "Manager note. A reason is mandatory when rejecting a submission.");
    decisionNoteEdit_->setMaximumHeight(82);
    actionLayout->addWidget(decisionNoteEdit_);

    auto* decisionButtons = new QHBoxLayout();
    approveButton_ = new QPushButton("Approve Proposed Score", this);
    rejectButton_ = new QPushButton("Reject", this);
    approveButton_->setEnabled(false);
    rejectButton_->setEnabled(false);
    decisionButtons->addWidget(approveButton_);
    decisionButtons->addWidget(rejectButton_);
    decisionButtons->addStretch();
    actionLayout->addLayout(decisionButtons);
    mainLayout->addWidget(actionGroup);

    connect(typeFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ManagerReviewWidget::applyFilters);
    connect(areaFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ManagerReviewWidget::applyFilters);
    connect(focusFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ManagerReviewWidget::applyFilters);
    connect(searchEdit_, &QLineEdit::textChanged, this, &ManagerReviewWidget::applyFilters);
    connect(refreshButton, &QPushButton::clicked, this, &ManagerReviewWidget::refresh);
    connect(table_, &QTableWidget::itemSelectionChanged, this, &ManagerReviewWidget::onSelectionChanged);
    connect(approveButton_, &QPushButton::clicked, this, [this]() { applyDecision("approved"); });
    connect(rejectButton_, &QPushButton::clicked, this, [this]() { applyDecision("rejected"); });
}

void ManagerReviewWidget::refresh()
{
    loadData();
}

void ManagerReviewWidget::loadData()
{
    QElapsedTimer timer;
    timer.start();
    submissions_ = submissionRepo_.findAll();
    submissionsById_.clear();

    QSet<QString> areaNames;
    for (const AssessmentSubmission& submission : submissions_) {
        submissionsById_.insert(submission.id, submission);
        areaNames.insert(submission.areaName);
    }

    const QString previousArea = areaFilterCombo_->currentData().toString();
    areaFilterCombo_->blockSignals(true);
    areaFilterCombo_->clear();
    areaFilterCombo_->addItem("All Areas", "all");
    QStringList sortedAreas = areaNames.values();
    sortedAreas.sort(Qt::CaseInsensitive);
    for (const QString& areaName : sortedAreas) {
        if (!areaName.isEmpty()) {
            areaFilterCombo_->addItem(areaName, areaName);
        }
    }
    const int previousIndex = areaFilterCombo_->findData(previousArea);
    areaFilterCombo_->setCurrentIndex(previousIndex >= 0 ? previousIndex : 0);
    areaFilterCombo_->blockSignals(false);

    rebuildTable();
    applyFilters();
    Logger::instance().info("ManagerReviewWidget",
        QString("Loaded %1 approval records in %2 ms")
            .arg(submissions_.size())
            .arg(timer.elapsed()));
}

void ManagerReviewWidget::rebuildTable()
{
    table_->setUpdatesEnabled(false);
    table_->setRowCount(submissions_.size());

    for (int row = 0; row < submissions_.size(); ++row) {
        const AssessmentSubmission& submission = submissions_.at(row);
        auto* engineerItem = new QTableWidgetItem(submission.engineerName);
        engineerItem->setData(Qt::UserRole, submission.id);
        table_->setItem(row, 0, engineerItem);
        table_->setItem(row, 1, new QTableWidgetItem(submission.shift));
        table_->setItem(row, 2, new QTableWidgetItem(displayType(submission.assessmentType)));
        table_->setItem(row, 3, new QTableWidgetItem(submission.areaName));
        table_->setItem(row, 4, new QTableWidgetItem(submission.machineName));
        table_->setItem(row, 5, new QTableWidgetItem(submission.competencyName));

        auto* approvedItem = new QTableWidgetItem(
            submission.hasBaseScore ? QString::number(submission.baseScore) : "Not assessed");
        approvedItem->setTextAlignment(Qt::AlignCenter);
        table_->setItem(row, 6, approvedItem);

        auto* proposedItem = new QTableWidgetItem(QString::number(submission.proposedScore));
        proposedItem->setTextAlignment(Qt::AlignCenter);
        QFont proposedFont = proposedItem->font();
        proposedFont.setBold(true);
        proposedItem->setFont(proposedFont);
        table_->setItem(row, 7, proposedItem);

        auto* decisionItem = new QTableWidgetItem(
            submission.decision.isEmpty() ? "pending" : submission.decision);
        decisionItem->setTextAlignment(Qt::AlignCenter);
        decisionItem->setBackground(decisionColour(submission.decision));
        table_->setItem(row, 8, decisionItem);

        table_->setItem(row, 9, new QTableWidgetItem(
            submission.submittedAt.toLocalTime().toString("dd MMM yyyy HH:mm")));
        table_->setItem(row, 10, new QTableWidgetItem(submission.reviewedByName));
    }

    table_->setUpdatesEnabled(true);
}

bool ManagerReviewWidget::matchesFilters(const AssessmentSubmission& submission) const
{
    const QString typeFilter = typeFilterCombo_->currentData().toString();
    const QString areaFilter = areaFilterCombo_->currentData().toString();
    const QString decisionFilter = focusFilterCombo_->currentData().toString();
    const QString search = searchEdit_->text().trimmed().toLower();

    if (typeFilter != "all" && submission.assessmentType != typeFilter) {
        return false;
    }
    if (areaFilter != "all" && submission.areaName != areaFilter) {
        return false;
    }
    if (decisionFilter != "all" && submission.decision != decisionFilter) {
        return false;
    }
    if (!search.isEmpty()) {
        const QString haystack = QString("%1 %2 %3 %4 %5 %6")
            .arg(submission.engineerName, submission.shift, submission.areaName,
                 submission.machineName, submission.competencyName, submission.decision)
            .toLower();
        if (!haystack.contains(search)) {
            return false;
        }
    }
    return true;
}

void ManagerReviewWidget::applyFilters()
{
    int visible = 0;
    int pending = 0;
    int approved = 0;
    int rejected = 0;

    for (int row = 0; row < submissions_.size(); ++row) {
        const AssessmentSubmission& submission = submissions_.at(row);
        const bool matches = matchesFilters(submission);
        table_->setRowHidden(row, !matches);
        if (!matches) {
            continue;
        }
        visible++;
        pending += submission.decision == "pending" ? 1 : 0;
        approved += submission.decision == "approved" ? 1 : 0;
        rejected += submission.decision == "rejected" ? 1 : 0;
    }

    summaryLabel_->setText(
        QString("Showing %1 | Pending %2 | Approved %3 | Rejected %4")
            .arg(visible).arg(pending).arg(approved).arg(rejected));
    onSelectionChanged();
}

QString ManagerReviewWidget::submissionIdForRow(int row) const
{
    const QTableWidgetItem* item = row >= 0 ? table_->item(row, 0) : nullptr;
    return item ? item->data(Qt::UserRole).toString() : QString();
}

void ManagerReviewWidget::onSelectionChanged()
{
    const int row = table_->currentRow();
    const QString submissionId = submissionIdForRow(row);
    if (submissionId.isEmpty() || !submissionsById_.contains(submissionId) || table_->isRowHidden(row)) {
        selectedItemLabel_->setText("Select a pending submission to review.");
        decisionNoteEdit_->clear();
        approveButton_->setEnabled(false);
        rejectButton_->setEnabled(false);
        return;
    }

    const AssessmentSubmission& submission = submissionsById_.value(submissionId);
    selectedItemLabel_->setText(
        QString("%1 | %2 | approved %3 -> proposed %4")
            .arg(submission.engineerName, submission.competencyName)
            .arg(submission.hasBaseScore ? QString::number(submission.baseScore) : "not assessed")
            .arg(submission.proposedScore));
    decisionNoteEdit_->setPlainText(submission.reviewNote);
    const bool isPending = submission.decision == "pending";
    approveButton_->setEnabled(isPending);
    rejectButton_->setEnabled(isPending);
}

void ManagerReviewWidget::applyDecision(const QString& decision)
{
    const int row = table_->currentRow();
    const QString submissionId = submissionIdForRow(row);
    if (submissionId.isEmpty() || !submissionsById_.contains(submissionId)) {
        QMessageBox::warning(this, "No Selection", "Select a pending submission first.");
        return;
    }

    const AssessmentSubmission submission = submissionsById_.value(submissionId);
    if (submission.decision != "pending") {
        QMessageBox::information(this, "Already Reviewed",
            "This submission is already final. A new engineer submission is required for another decision.");
        return;
    }

    const QString note = decisionNoteEdit_->toPlainText().trimmed();
    if (decision == "rejected" && note.isEmpty()) {
        QMessageBox::warning(this, "Rejection Reason Required",
            "Enter a clear reason so the engineer knows what must change.");
        decisionNoteEdit_->setFocus();
        return;
    }

    Session* session = Application::instance().session();
    if (!session || !session->isAdmin()) {
        QMessageBox::warning(this, "Permission Denied",
            "An authenticated manager is required to review submissions.");
        return;
    }
    const QString reviewerUserId = session ? session->userId() : QString();
    const QString reviewerName = session ? session->username() : QString("manager");

    if (!submissionRepo_.decide(
            submission.id, submission.rowVersion, decision, note,
            reviewerUserId, reviewerName)) {
        QMessageBox::warning(this, "Decision Not Saved", submissionRepo_.lastError());
        loadData();
        return;
    }

    Logger::instance().info("ManagerReviewWidget",
        QString("%1 submission %2 by %3")
            .arg(decision, submission.id, reviewerName));
    decisionNoteEdit_->clear();
    loadData();
}
