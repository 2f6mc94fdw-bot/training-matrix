#include "AssessmentWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>

AssessmentWidget::AssessmentWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCombo_(nullptr)
    , productionAreaCombo_(nullptr)
    , assessmentTable_(nullptr)
    , saveButton_(nullptr)
    , loadButton_(nullptr)
{
    setupUI();
    loadEngineers();
    loadProductionAreas();
    Logger::instance().info("AssessmentWidget", "Assessment widget initialized");
}

AssessmentWidget::~AssessmentWidget()
{
}

void AssessmentWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Title
    QLabel* titleLabel = new QLabel("Competency Assessment", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(8);

    // Selection Group
    QGroupBox* selectionGroup = new QGroupBox("Select Engineer and Production Area", this);
    QFormLayout* formLayout = new QFormLayout(selectionGroup);
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(20, 20, 20, 20);

    engineerCombo_ = new QComboBox(this);
    productionAreaCombo_ = new QComboBox(this);

    connect(engineerCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AssessmentWidget::onEngineerChanged);
    connect(productionAreaCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AssessmentWidget::onProductionAreaChanged);

    formLayout->addRow("Engineer:", engineerCombo_);
    formLayout->addRow("Production Area:", productionAreaCombo_);

    mainLayout->addWidget(selectionGroup);
    mainLayout->addSpacing(8);

    // Assessment Table
    QLabel* tableLabel = new QLabel("Competencies (0 = No skill, 1 = Basic, 2 = Intermediate, 3 = Advanced)", this);
    QFont labelFont = tableLabel->font();
    labelFont.setPointSize(11);
    tableLabel->setFont(labelFont);
    mainLayout->addWidget(tableLabel);

    assessmentTable_ = new QTableWidget(this);
    assessmentTable_->setColumnCount(4);
    assessmentTable_->setHorizontalHeaderLabels({"Machine", "Competency", "Score", "IDs"});
    assessmentTable_->setAlternatingRowColors(true);
    assessmentTable_->hideColumn(3); // Hide IDs column

    // Set proper column widths for readability
    assessmentTable_->horizontalHeader()->setStretchLastSection(false);
    assessmentTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    assessmentTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    assessmentTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);

    // Set initial column widths
    assessmentTable_->setColumnWidth(0, 200); // Machine column
    assessmentTable_->setColumnWidth(2, 100); // Score column

    // Enable word wrap for better text display
    assessmentTable_->setWordWrap(true);
    assessmentTable_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    mainLayout->addWidget(assessmentTable_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    loadButton_ = new QPushButton("Load Existing Assessments", this);
    saveButton_ = new QPushButton("Save Assessments", this);

    // Set minimum button widths for better UX
    loadButton_->setMinimumWidth(180);
    saveButton_->setMinimumWidth(180);

    connect(loadButton_, &QPushButton::clicked, this, &AssessmentWidget::onLoadClicked);
    connect(saveButton_, &QPushButton::clicked, this, &AssessmentWidget::onSaveClicked);

    buttonLayout->addWidget(loadButton_);
    buttonLayout->addWidget(saveButton_);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void AssessmentWidget::loadEngineers()
{
    engineerCombo_->clear();
    engineerCombo_->addItem("-- Select Engineer --", "");

    QList<Engineer> engineers = engineerRepo_.findAll();

    for (const Engineer& engineer : engineers) {
        engineerCombo_->addItem(engineer.name(), engineer.id());
    }
}

void AssessmentWidget::loadProductionAreas()
{
    productionAreaCombo_->clear();
    productionAreaCombo_->addItem("-- Select Production Area --", 0);

    QList<ProductionArea> areas = productionRepo_.findAllAreas();

    for (const ProductionArea& area : areas) {
        productionAreaCombo_->addItem(area.name(), area.id());
    }
}

void AssessmentWidget::loadAssessmentData()
{
    assessmentTable_->setRowCount(0);

    QString engineerId = engineerCombo_->currentData().toString();
    int areaId = productionAreaCombo_->currentData().toInt();

    if (engineerId.isEmpty() || areaId == 0) {
        return;
    }

    // Load machines for the selected area
    QList<Machine> machines = productionRepo_.findMachinesByArea(areaId);

    int row = 0;
    for (const Machine& machine : machines) {
        QList<Competency> competencies = productionRepo_.findCompetenciesByMachine(machine.id());

        for (const Competency& competency : competencies) {
            assessmentTable_->insertRow(row);

            assessmentTable_->setItem(row, 0, new QTableWidgetItem(machine.name()));
            assessmentTable_->setItem(row, 1, new QTableWidgetItem(competency.name()));

            // Score spinner
            QSpinBox* scoreSpinner = new QSpinBox(this);
            scoreSpinner->setRange(0, 3);
            scoreSpinner->setValue(0);
            assessmentTable_->setCellWidget(row, 2, scoreSpinner);

            // Store IDs (hidden)
            QString ids = QString("%1|%2|%3").arg(areaId).arg(machine.id()).arg(competency.id());
            assessmentTable_->setItem(row, 3, new QTableWidgetItem(ids));

            row++;
        }
    }

    Logger::instance().info("AssessmentWidget", QString("Loaded %1 competencies for assessment").arg(row));
}

void AssessmentWidget::onEngineerChanged(int index)
{
    if (index > 0 && productionAreaCombo_->currentIndex() > 0) {
        loadAssessmentData();
    }
}

void AssessmentWidget::onProductionAreaChanged(int index)
{
    if (index > 0 && engineerCombo_->currentIndex() > 0) {
        loadAssessmentData();
    }
}

void AssessmentWidget::onLoadClicked()
{
    QString engineerId = engineerCombo_->currentData().toString();
    int areaId = productionAreaCombo_->currentData().toInt();

    if (engineerId.isEmpty() || areaId == 0) {
        QMessageBox::warning(this, "Selection Required", "Please select both an engineer and a production area.");
        return;
    }

    // Load existing assessments for this engineer
    QList<Assessment> assessments = assessmentRepo_.findByEngineer(engineerId);

    // Update table with existing scores
    for (int row = 0; row < assessmentTable_->rowCount(); ++row) {
        QString ids = assessmentTable_->item(row, 3)->text();
        QStringList parts = ids.split('|');

        if (parts.size() == 3) {
            int machineId = parts[1].toInt();
            int competencyId = parts[2].toInt();

            // Find matching assessment
            for (const Assessment& assessment : assessments) {
                if (assessment.machineId() == machineId &&
                    assessment.competencyId() == competencyId &&
                    assessment.productionAreaId() == areaId) {

                    QSpinBox* spinner = qobject_cast<QSpinBox*>(assessmentTable_->cellWidget(row, 2));
                    if (spinner) {
                        spinner->setValue(assessment.score());
                    }
                    break;
                }
            }
        }
    }

    Logger::instance().info("AssessmentWidget", "Loaded existing assessments for engineer: " + engineerId);
    QMessageBox::information(this, "Success", "Existing assessments loaded successfully.");
}

void AssessmentWidget::onSaveClicked()
{
    QString engineerId = engineerCombo_->currentData().toString();
    int areaId = productionAreaCombo_->currentData().toInt();

    if (engineerId.isEmpty() || areaId == 0) {
        QMessageBox::warning(this, "Selection Required", "Please select both an engineer and a production area.");
        return;
    }

    int savedCount = 0;
    int errorCount = 0;

    for (int row = 0; row < assessmentTable_->rowCount(); ++row) {
        QString ids = assessmentTable_->item(row, 3)->text();
        QStringList parts = ids.split('|');

        if (parts.size() == 3) {
            int machineId = parts[1].toInt();
            int competencyId = parts[2].toInt();

            QSpinBox* spinner = qobject_cast<QSpinBox*>(assessmentTable_->cellWidget(row, 2));
            if (spinner) {
                int score = spinner->value();

                Assessment assessment(0, engineerId, areaId, machineId, competencyId, score);

                if (assessmentRepo_.saveOrUpdate(assessment)) {
                    savedCount++;
                } else {
                    errorCount++;
                    Logger::instance().error("AssessmentWidget",
                        QString("Failed to save assessment: %1").arg(assessmentRepo_.lastError()));
                }
            }
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Saved %1 assessments, but %2 failed.").arg(savedCount).arg(errorCount));
    } else {
        Logger::instance().info("AssessmentWidget", QString("Saved %1 assessments").arg(savedCount));
        QMessageBox::information(this, "Success",
            QString("Successfully saved %1 assessments.").arg(savedCount));
    }
}
