#include "EngineersWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>

EngineersWidget::EngineersWidget(QWidget* parent)
    : QWidget(parent)
    , tableWidget_(nullptr)
    , addButton_(nullptr)
    , editButton_(nullptr)
    , deleteButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadEngineers();
    Logger::instance().info("EngineersWidget", "Engineers widget initialized");
}

EngineersWidget::~EngineersWidget()
{
}

void EngineersWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel("Engineers Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Table
    tableWidget_ = new QTableWidget(this);
    tableWidget_->setColumnCount(3);
    tableWidget_->setHorizontalHeaderLabels({"ID", "Name", "Shift"});
    tableWidget_->horizontalHeader()->setStretchLastSection(true);
    tableWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_->setAlternatingRowColors(true);

    connect(tableWidget_, &QTableWidget::cellDoubleClicked, this, &EngineersWidget::onTableDoubleClicked);

    mainLayout->addWidget(tableWidget_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    addButton_ = new QPushButton("Add Engineer", this);
    editButton_ = new QPushButton("Edit Engineer", this);
    deleteButton_ = new QPushButton("Delete Engineer", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(addButton_, &QPushButton::clicked, this, &EngineersWidget::onAddClicked);
    connect(editButton_, &QPushButton::clicked, this, &EngineersWidget::onEditClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &EngineersWidget::onDeleteClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &EngineersWidget::onRefreshClicked);

    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void EngineersWidget::loadEngineers()
{
    tableWidget_->setRowCount(0);

    QList<Engineer> engineers = repository_.findAll();

    if (!repository_.lastError().isEmpty()) {
        Logger::instance().error("EngineersWidget", "Failed to load engineers: " + repository_.lastError());
        QMessageBox::critical(this, "Error", "Failed to load engineers: " + repository_.lastError());
        return;
    }

    tableWidget_->setRowCount(engineers.size());

    for (int i = 0; i < engineers.size(); ++i) {
        const Engineer& engineer = engineers[i];

        tableWidget_->setItem(i, 0, new QTableWidgetItem(engineer.id()));
        tableWidget_->setItem(i, 1, new QTableWidgetItem(engineer.name()));
        tableWidget_->setItem(i, 2, new QTableWidgetItem(engineer.shift()));
    }

    Logger::instance().info("EngineersWidget", QString("Loaded %1 engineers").arg(engineers.size()));
}

void EngineersWidget::showEngineerDialog(const Engineer* engineer)
{
    QDialog dialog(this);
    dialog.setWindowTitle(engineer ? "Edit Engineer" : "Add Engineer");
    dialog.resize(400, 200);

    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* idEdit = new QLineEdit(&dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    QComboBox* shiftCombo = new QComboBox(&dialog);

    shiftCombo->addItems({"A", "B", "C", "D", "Days"});

    if (engineer) {
        idEdit->setText(engineer->id());
        idEdit->setReadOnly(true);
        nameEdit->setText(engineer->name());
        shiftCombo->setCurrentText(engineer->shift());
    }

    formLayout->addRow("ID:", idEdit);
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Shift:", shiftCombo);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString id = idEdit->text().trimmed();
        QString name = nameEdit->text().trimmed();
        QString shift = shiftCombo->currentText();

        if (id.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "ID and Name are required.");
            return;
        }

        bool success = false;
        if (engineer) {
            // Update existing
            Engineer updatedEngineer(id, name, shift);
            success = repository_.update(updatedEngineer);

            if (success) {
                Logger::instance().info("EngineersWidget", "Updated engineer: " + id);
                QMessageBox::information(this, "Success", "Engineer updated successfully.");
            }
        } else {
            // Create new
            Engineer newEngineer(id, name, shift);
            success = repository_.save(newEngineer);

            if (success) {
                Logger::instance().info("EngineersWidget", "Created engineer: " + id);
                QMessageBox::information(this, "Success", "Engineer created successfully.");
            }
        }

        if (!success) {
            Logger::instance().error("EngineersWidget", "Failed to save engineer: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save engineer: " + repository_.lastError());
        } else {
            loadEngineers();
        }
    }
}

void EngineersWidget::onAddClicked()
{
    showEngineerDialog(nullptr);
}

void EngineersWidget::onEditClicked()
{
    int currentRow = tableWidget_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an engineer to edit.");
        return;
    }

    QString id = tableWidget_->item(currentRow, 0)->text();
    Engineer engineer = repository_.findById(id);

    if (engineer.id().isEmpty()) {
        QMessageBox::critical(this, "Error", "Failed to load engineer data.");
        return;
    }

    showEngineerDialog(&engineer);
}

void EngineersWidget::onDeleteClicked()
{
    int currentRow = tableWidget_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an engineer to delete.");
        return;
    }

    QString id = tableWidget_->item(currentRow, 0)->text();
    QString name = tableWidget_->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete engineer '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        bool success = repository_.remove(id);

        if (success) {
            Logger::instance().info("EngineersWidget", "Deleted engineer: " + id);
            QMessageBox::information(this, "Success", "Engineer deleted successfully.");
            loadEngineers();
        } else {
            Logger::instance().error("EngineersWidget", "Failed to delete engineer: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to delete engineer: " + repository_.lastError());
        }
    }
}

void EngineersWidget::onRefreshClicked()
{
    loadEngineers();
}

void EngineersWidget::onTableDoubleClicked(int row, int column)
{
    QString id = tableWidget_->item(row, 0)->text();
    Engineer engineer = repository_.findById(id);

    if (engineer.id().isEmpty()) {
        QMessageBox::critical(this, "Error", "Failed to load engineer data.");
        return;
    }

    showEngineerDialog(&engineer);
}
