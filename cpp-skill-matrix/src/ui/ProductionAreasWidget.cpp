#include "ProductionAreasWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>

ProductionAreasWidget::ProductionAreasWidget(QWidget* parent)
    : QWidget(parent)
    , treeWidget_(nullptr)
    , addButton_(nullptr)
    , editButton_(nullptr)
    , deleteButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadProductionAreas();
    Logger::instance().info("ProductionAreasWidget", "Production Areas widget initialized");
}

ProductionAreasWidget::~ProductionAreasWidget()
{
}

void ProductionAreasWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Title
    QLabel* titleLabel = new QLabel("Production Areas Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(8);

    // Tree
    treeWidget_ = new QTreeWidget(this);
    treeWidget_->setHeaderLabels({"Name", "Type", "ID"});
    treeWidget_->setAlternatingRowColors(true);
    treeWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_->setIndentation(24);

    // Set proper column widths
    treeWidget_->header()->setStretchLastSection(false);
    treeWidget_->header()->setSectionResizeMode(0, QHeaderView::Stretch); // Name takes remaining space
    treeWidget_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Type fits content
    treeWidget_->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // ID fits content

    connect(treeWidget_, &QTreeWidget::itemDoubleClicked, this, &ProductionAreasWidget::onTreeItemDoubleClicked);

    mainLayout->addWidget(treeWidget_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    addButton_ = new QPushButton("Add", this);
    editButton_ = new QPushButton("Edit", this);
    deleteButton_ = new QPushButton("Delete", this);
    refreshButton_ = new QPushButton("Refresh", this);

    // Set minimum button widths for better UX
    addButton_->setMinimumWidth(100);
    editButton_->setMinimumWidth(100);
    deleteButton_->setMinimumWidth(100);
    refreshButton_->setMinimumWidth(100);

    connect(addButton_, &QPushButton::clicked, this, &ProductionAreasWidget::onAddClicked);
    connect(editButton_, &QPushButton::clicked, this, &ProductionAreasWidget::onEditClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &ProductionAreasWidget::onDeleteClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &ProductionAreasWidget::onRefreshClicked);

    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void ProductionAreasWidget::loadProductionAreas()
{
    treeWidget_->clear();

    QList<ProductionArea> areas = repository_.findAllAreas();

    if (!repository_.lastError().isEmpty()) {
        Logger::instance().error("ProductionAreasWidget", "Failed to load areas: " + repository_.lastError());
        QMessageBox::critical(this, "Error", "Failed to load production areas: " + repository_.lastError());
        return;
    }

    for (const ProductionArea& area : areas) {
        QTreeWidgetItem* areaItem = new QTreeWidgetItem(treeWidget_);
        areaItem->setText(0, area.name());
        areaItem->setText(1, "Production Area");
        areaItem->setText(2, QString::number(area.id()));
        areaItem->setData(0, Qt::UserRole, AreaItem);

        // Load machines for this area
        QList<Machine> machines = repository_.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QTreeWidgetItem* machineItem = new QTreeWidgetItem(areaItem);
            machineItem->setText(0, machine.name());
            machineItem->setText(1, "Machine");
            machineItem->setText(2, QString::number(machine.id()));
            machineItem->setData(0, Qt::UserRole, MachineItem);

            // Load competencies for this machine
            QList<Competency> competencies = repository_.findCompetenciesByMachine(machine.id());
            for (const Competency& competency : competencies) {
                QTreeWidgetItem* competencyItem = new QTreeWidgetItem(machineItem);
                competencyItem->setText(0, competency.name());
                competencyItem->setText(1, "Competency");
                competencyItem->setText(2, QString::number(competency.id()));
                competencyItem->setData(0, Qt::UserRole, CompetencyItem);
            }
        }

        areaItem->setExpanded(true);
    }

    Logger::instance().info("ProductionAreasWidget", QString("Loaded %1 production areas").arg(areas.size()));
}

void ProductionAreasWidget::showAreaDialog(const ProductionArea* area)
{
    QDialog dialog(this);
    dialog.setWindowTitle(area ? "Edit Production Area" : "Add Production Area");
    dialog.resize(400, 150);

    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* nameEdit = new QLineEdit(&dialog);

    if (area) {
        nameEdit->setText(area->name());
    }

    formLayout->addRow("Name:", nameEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Name is required.");
            return;
        }

        bool success = false;
        if (area) {
            ProductionArea updatedArea(area->id(), name);
            success = repository_.updateArea(updatedArea);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Updated area: " + name);
                QMessageBox::information(this, "Success", "Production area updated successfully.");
            }
        } else {
            ProductionArea newArea(0, name);
            success = repository_.saveArea(newArea);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Created area: " + name);
                QMessageBox::information(this, "Success", "Production area created successfully.");
            }
        }

        if (!success) {
            Logger::instance().error("ProductionAreasWidget", "Failed to save area: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save production area: " + repository_.lastError());
        } else {
            loadProductionAreas();
        }
    }
}

void ProductionAreasWidget::showMachineDialog(int parentAreaId, const Machine* machine)
{
    QDialog dialog(this);
    dialog.setWindowTitle(machine ? "Edit Machine" : "Add Machine");
    dialog.resize(400, 200);

    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* nameEdit = new QLineEdit(&dialog);
    QSpinBox* importanceSpin = new QSpinBox(&dialog);
    importanceSpin->setRange(1, 10);
    importanceSpin->setValue(1);

    if (machine) {
        nameEdit->setText(machine->name());
        importanceSpin->setValue(machine->importance());
    }

    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Importance (1-10):", importanceSpin);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        int importance = importanceSpin->value();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Name is required.");
            return;
        }

        bool success = false;
        if (machine) {
            Machine updatedMachine(machine->id(), parentAreaId, name, importance);
            success = repository_.updateMachine(updatedMachine);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Updated machine: " + name);
                QMessageBox::information(this, "Success", "Machine updated successfully.");
            }
        } else {
            Machine newMachine(0, parentAreaId, name, importance);
            success = repository_.saveMachine(newMachine);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Created machine: " + name);
                QMessageBox::information(this, "Success", "Machine created successfully.");
            }
        }

        if (!success) {
            Logger::instance().error("ProductionAreasWidget", "Failed to save machine: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save machine: " + repository_.lastError());
        } else {
            loadProductionAreas();
        }
    }
}

void ProductionAreasWidget::showCompetencyDialog(int parentMachineId, const Competency* competency)
{
    QDialog dialog(this);
    dialog.setWindowTitle(competency ? "Edit Competency" : "Add Competency");
    dialog.resize(400, 200);

    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* nameEdit = new QLineEdit(&dialog);
    QSpinBox* maxScoreSpin = new QSpinBox(&dialog);
    maxScoreSpin->setRange(1, 10);
    maxScoreSpin->setValue(3);

    if (competency) {
        nameEdit->setText(competency->name());
        maxScoreSpin->setValue(competency->maxScore());
    }

    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Max Score:", maxScoreSpin);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        int maxScore = maxScoreSpin->value();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Name is required.");
            return;
        }

        bool success = false;
        if (competency) {
            Competency updatedCompetency(competency->id(), parentMachineId, name, maxScore);
            success = repository_.updateCompetency(updatedCompetency);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Updated competency: " + name);
                QMessageBox::information(this, "Success", "Competency updated successfully.");
            }
        } else {
            Competency newCompetency(0, parentMachineId, name, maxScore);
            success = repository_.saveCompetency(newCompetency);
            if (success) {
                Logger::instance().info("ProductionAreasWidget", "Created competency: " + name);
                QMessageBox::information(this, "Success", "Competency created successfully.");
            }
        }

        if (!success) {
            Logger::instance().error("ProductionAreasWidget", "Failed to save competency: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save competency: " + repository_.lastError());
        } else {
            loadProductionAreas();
        }
    }
}

void ProductionAreasWidget::onAddClicked()
{
    QTreeWidgetItem* currentItem = treeWidget_->currentItem();

    if (!currentItem) {
        // No selection - add production area
        showAreaDialog(nullptr);
    } else {
        int itemType = currentItem->data(0, Qt::UserRole).toInt();

        if (itemType == AreaItem) {
            // Add machine to this area
            int areaId = currentItem->text(2).toInt();
            showMachineDialog(areaId, nullptr);
        } else if (itemType == MachineItem) {
            // Add competency to this machine
            int machineId = currentItem->text(2).toInt();
            showCompetencyDialog(machineId, nullptr);
        } else {
            QMessageBox::information(this, "Info", "Cannot add items to a competency. Select a production area or machine.");
        }
    }
}

void ProductionAreasWidget::onEditClicked()
{
    QTreeWidgetItem* currentItem = treeWidget_->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to edit.");
        return;
    }

    int itemType = currentItem->data(0, Qt::UserRole).toInt();
    int id = currentItem->text(2).toInt();

    if (itemType == AreaItem) {
        ProductionArea area = repository_.findAreaById(id);
        if (area.id() > 0) {
            showAreaDialog(&area);
        }
    } else if (itemType == MachineItem) {
        Machine machine = repository_.findMachineById(id);
        if (machine.id() > 0) {
            showMachineDialog(machine.productionAreaId(), &machine);
        }
    } else if (itemType == CompetencyItem) {
        Competency competency = repository_.findCompetencyById(id);
        if (competency.id() > 0) {
            showCompetencyDialog(competency.machineId(), &competency);
        }
    }
}

void ProductionAreasWidget::onDeleteClicked()
{
    QTreeWidgetItem* currentItem = treeWidget_->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to delete.");
        return;
    }

    int itemType = currentItem->data(0, Qt::UserRole).toInt();
    int id = currentItem->text(2).toInt();
    QString name = currentItem->text(0);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete '%1'?\nThis will also delete all child items.").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        bool success = false;

        if (itemType == AreaItem) {
            success = repository_.removeArea(id);
        } else if (itemType == MachineItem) {
            success = repository_.removeMachine(id);
        } else if (itemType == CompetencyItem) {
            success = repository_.removeCompetency(id);
        }

        if (success) {
            Logger::instance().info("ProductionAreasWidget", "Deleted item: " + name);
            QMessageBox::information(this, "Success", "Item deleted successfully.");
            loadProductionAreas();
        } else {
            Logger::instance().error("ProductionAreasWidget", "Failed to delete: " + repository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to delete item: " + repository_.lastError());
        }
    }
}

void ProductionAreasWidget::onRefreshClicked()
{
    loadProductionAreas();
}

void ProductionAreasWidget::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (!item) return;

    int itemType = item->data(0, Qt::UserRole).toInt();
    int id = item->text(2).toInt();

    if (itemType == AreaItem) {
        ProductionArea area = repository_.findAreaById(id);
        if (area.id() > 0) {
            showAreaDialog(&area);
        }
    } else if (itemType == MachineItem) {
        Machine machine = repository_.findMachineById(id);
        if (machine.id() > 0) {
            showMachineDialog(machine.productionAreaId(), &machine);
        }
    } else if (itemType == CompetencyItem) {
        Competency competency = repository_.findCompetencyById(id);
        if (competency.id() > 0) {
            showCompetencyDialog(competency.machineId(), &competency);
        }
    }
}
