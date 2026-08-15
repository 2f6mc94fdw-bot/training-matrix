#include "SnapshotsWidget.h"
#include "../database/AssessmentRepository.h"
#include "../database/CoreSkillsRepository.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SnapshotsWidget::SnapshotsWidget(QWidget* parent)
    : QWidget(parent)
    , snapshotList_(nullptr)
    , createButton_(nullptr)
    , deleteButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadSnapshots();
    Logger::instance().info("SnapshotsWidget", "Snapshots widget initialized");
}

SnapshotsWidget::~SnapshotsWidget()
{
}

void SnapshotsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Snapshot Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel* infoLabel = new QLabel("Snapshots capture the current state of all assessments for historical tracking.", this);
    mainLayout->addWidget(infoLabel);

    snapshotList_ = new QListWidget(this);
    mainLayout->addWidget(snapshotList_);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    createButton_ = new QPushButton("Create Snapshot", this);
    deleteButton_ = new QPushButton("Delete Snapshot", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(createButton_, &QPushButton::clicked, this, &SnapshotsWidget::onCreateSnapshotClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &SnapshotsWidget::onDeleteSnapshotClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &SnapshotsWidget::onRefreshClicked);

    buttonLayout->addWidget(createButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SnapshotsWidget::loadSnapshots()
{
    snapshotList_->clear();

    QList<Snapshot> snapshots = snapshotRepo_.findAll();

    for (const Snapshot& snapshot : snapshots) {
        QString displayText = QString("%1 - %2")
            .arg(snapshot.timestamp().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(snapshot.description());

        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, snapshot.id());
        snapshotList_->addItem(item);
    }

    Logger::instance().info("SnapshotsWidget", QString("Loaded %1 snapshots").arg(snapshots.size()));
}

void SnapshotsWidget::onCreateSnapshotClicked()
{
    bool ok;
    QString description = QInputDialog::getText(this, "Create Snapshot",
        "Enter snapshot description:", QLineEdit::Normal, "", &ok);

    if (ok && !description.isEmpty()) {
        AssessmentRepository assessmentRepo;
        CoreSkillsRepository coreSkillsRepo;
        const QList<Assessment> assessments = assessmentRepo.findAll();
        const QList<CoreSkillAssessment> coreSkillAssessments = coreSkillsRepo.findAllAssessments();

        QJsonArray assessmentsArray;
        for (const Assessment& assessment : assessments) {
            QJsonObject item;
            item["id"] = assessment.id();
            item["engineerId"] = assessment.engineerId();
            item["productionAreaId"] = assessment.productionAreaId();
            item["machineId"] = assessment.machineId();
            item["competencyId"] = assessment.competencyId();
            item["score"] = assessment.score();
            item["createdAt"] = assessment.createdAt().toString(Qt::ISODate);
            item["updatedAt"] = assessment.updatedAt().toString(Qt::ISODate);
            assessmentsArray.append(item);
        }

        QJsonArray coreAssessmentsArray;
        for (const CoreSkillAssessment& assessment : coreSkillAssessments) {
            QJsonObject item;
            item["id"] = assessment.id();
            item["engineerId"] = assessment.engineerId();
            item["categoryId"] = assessment.categoryId();
            item["skillId"] = assessment.skillId();
            item["score"] = assessment.score();
            item["createdAt"] = assessment.createdAt().toString(Qt::ISODate);
            item["updatedAt"] = assessment.updatedAt().toString(Qt::ISODate);
            coreAssessmentsArray.append(item);
        }

        QJsonObject snapshotData;
        snapshotData["assessments"] = assessmentsArray;
        snapshotData["coreSkillAssessments"] = coreAssessmentsArray;
        snapshotData["capturedAt"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        Snapshot snapshot;
        snapshot.setDescription(description);
        snapshot.setTimestamp(QDateTime::currentDateTime());
        snapshot.setData(QString::fromUtf8(QJsonDocument(snapshotData).toJson(QJsonDocument::Compact)));

        if (snapshotRepo_.save(snapshot)) {
            Logger::instance().info("SnapshotsWidget", "Created snapshot: " + description);
            QMessageBox::information(this, "Success", "Snapshot created successfully.");
            loadSnapshots();
        } else {
            Logger::instance().error("SnapshotsWidget", "Failed to create snapshot: " + snapshotRepo_.lastError());
            QMessageBox::critical(this, "Error", "Failed to create snapshot: " + snapshotRepo_.lastError());
        }
    }
}

void SnapshotsWidget::onDeleteSnapshotClicked()
{
    QListWidgetItem* currentItem = snapshotList_->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select a snapshot to delete.");
        return;
    }

    QString id = currentItem->data(Qt::UserRole).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        "Are you sure you want to delete this snapshot?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (snapshotRepo_.remove(id)) {
            Logger::instance().info("SnapshotsWidget", "Deleted snapshot: " + id);
            QMessageBox::information(this, "Success", "Snapshot deleted successfully.");
            loadSnapshots();
        } else {
            Logger::instance().error("SnapshotsWidget", "Failed to delete snapshot: " + snapshotRepo_.lastError());
            QMessageBox::critical(this, "Error", "Failed to delete snapshot: " + snapshotRepo_.lastError());
        }
    }
}

void SnapshotsWidget::onRefreshClicked()
{
    loadSnapshots();
}
