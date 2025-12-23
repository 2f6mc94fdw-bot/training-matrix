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
#include <QListWidget>

ProductionAreasWidget::ProductionAreasWidget(QWidget* parent)
    : QWidget(parent)
    , areaFilterCombo_(nullptr)
    , treeWidget_(nullptr)
    , addButton_(nullptr)
    , editButton_(nullptr)
    , deleteButton_(nullptr)
    , refreshButton_(nullptr)
    , manageAreasButton_(nullptr)
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

    // Area Filter Row
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(12);

    QLabel* filterLabel = new QLabel("Production Area:", this);
    QFont filterFont = filterLabel->font();
    filterFont.setPointSize(12);
    filterFont.setBold(true);
    filterLabel->setFont(filterFont);

    areaFilterCombo_ = new QComboBox(this);
    areaFilterCombo_->setMinimumWidth(300);
    areaFilterCombo_->setMinimumHeight(35);
    connect(areaFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProductionAreasWidget::onAreaFilterChanged);

    manageAreasButton_ = new QPushButton("Manage Areas", this);
    manageAreasButton_->setMinimumWidth(130);
    manageAreasButton_->setMinimumHeight(35);
    connect(manageAreasButton_, &QPushButton::clicked, [this]() {
        showAreaManagementDialog();
    });

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(areaFilterCombo_);
    filterLayout->addWidget(manageAreasButton_);
    filterLayout->addStretch();

    mainLayout->addLayout(filterLayout);
    mainLayout->addSpacing(8);

    // Tree
    treeWidget_ = new QTreeWidget(this);
    treeWidget_->setHeaderLabels({"Name", "Type", "Details", "ID"});
    treeWidget_->setAlternatingRowColors(true);
    treeWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_->setIndentation(24);
    treeWidget_->setUniformRowHeights(true);  // Better performance and consistent look

    // Set proper column widths
    treeWidget_->header()->setStretchLastSection(false);
    treeWidget_->header()->setSectionResizeMode(0, QHeaderView::Stretch); // Name takes remaining space
    treeWidget_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Type fits content
    treeWidget_->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Details fits content
    treeWidget_->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // ID fits content

    connect(treeWidget_, &QTreeWidget::itemDoubleClicked, this, &ProductionAreasWidget::onTreeItemDoubleClicked);

    mainLayout->addWidget(treeWidget_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    addButton_ = new QPushButton("Add Machine/Competency", this);
    editButton_ = new QPushButton("Edit", this);
    deleteButton_ = new QPushButton("Delete", this);
    refreshButton_ = new QPushButton("Refresh", this);

    // Set minimum button widths for better UX
    addButton_->setMinimumWidth(180);
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
    allAreas_ = repository_.findAllAreas();

    if (!repository_.lastError().isEmpty()) {
        Logger::instance().error("ProductionAreasWidget", "Failed to load areas: " + repository_.lastError());
        QMessageBox::critical(this, "Error", "Failed to load production areas: " + repository_.lastError());
        return;
    }

    loadAreaFilter();

    Logger::instance().info("ProductionAreasWidget", QString("Loaded %1 production areas").arg(allAreas_.size()));
}

void ProductionAreasWidget::loadAreaFilter()
{
    // Block signals while updating combo box
    areaFilterCombo_->blockSignals(true);

    int currentIndex = areaFilterCombo_->currentIndex();
    int currentAreaId = -1;
    if (currentIndex >= 0) {
        currentAreaId = areaFilterCombo_->currentData().toInt();
    }

    areaFilterCombo_->clear();

    // Add "All Areas" option
    areaFilterCombo_->addItem("All Areas", -1);

    // Add each production area
    for (const ProductionArea& area : allAreas_) {
        areaFilterCombo_->addItem(area.name(), area.id());
    }

    // Restore previous selection if possible
    if (currentAreaId >= 0) {
        int index = areaFilterCombo_->findData(currentAreaId);
        if (index >= 0) {
            areaFilterCombo_->setCurrentIndex(index);
        }
    }

    areaFilterCombo_->blockSignals(false);

    // Load data for current selection
    onAreaFilterChanged(areaFilterCombo_->currentIndex());
}

void ProductionAreasWidget::onAreaFilterChanged(int index)
{
    if (index < 0) return;

    int areaId = areaFilterCombo_->currentData().toInt();

    if (areaId == -1) {
        // Show all areas
        loadMachinesForArea(-1);
    } else {
        // Show specific area
        loadMachinesForArea(areaId);
    }
}

void ProductionAreasWidget::loadMachinesForArea(int areaId)
{
    treeWidget_->clear();

    if (areaId == -1) {
        // Show all areas with their machines
        for (const ProductionArea& area : allAreas_) {
            QTreeWidgetItem* areaItem = new QTreeWidgetItem(treeWidget_);
            areaItem->setText(0, area.name());
            areaItem->setText(1, "Production Area");
            areaItem->setText(2, "");
            areaItem->setText(3, QString::number(area.id()));
            areaItem->setData(0, Qt::UserRole, AreaItem);

            QFont areaFont = areaItem->font(0);
            areaFont.setBold(true);
            areaItem->setFont(0, areaFont);

            // Load machines for this area
            QList<Machine> machines = repository_.findMachinesByArea(area.id());
            for (const Machine& machine : machines) {
                QTreeWidgetItem* machineItem = new QTreeWidgetItem(areaItem);
                machineItem->setText(0, machine.name());
                machineItem->setText(1, "Machine");
                machineItem->setText(2, QString("Importance: %1").arg(machine.importance()));
                machineItem->setText(3, QString::number(machine.id()));
                machineItem->setData(0, Qt::UserRole, MachineItem);

                // Load competencies for this machine
                QList<Competency> competencies = repository_.findCompetenciesByMachine(machine.id());
                for (const Competency& competency : competencies) {
                    QTreeWidgetItem* competencyItem = new QTreeWidgetItem(machineItem);
                    competencyItem->setText(0, competency.name());
                    competencyItem->setText(1, "Competency");
                    competencyItem->setText(2, QString("Max Score: %1").arg(competency.maxScore()));
                    competencyItem->setText(3, QString::number(competency.id()));
                    competencyItem->setData(0, Qt::UserRole, CompetencyItem);
                }

                machineItem->setExpanded(true);
            }

            areaItem->setExpanded(true);
        }
    } else {
        // Show only machines for selected area
        QList<Machine> machines = repository_.findMachinesByArea(areaId);

        for (const Machine& machine : machines) {
            QTreeWidgetItem* machineItem = new QTreeWidgetItem(treeWidget_);
            machineItem->setText(0, machine.name());
            machineItem->setText(1, "Machine");
            machineItem->setText(2, QString("Importance: %1").arg(machine.importance()));
            machineItem->setText(3, QString::number(machine.id()));
            machineItem->setData(0, Qt::UserRole, MachineItem);

            QFont machineFont = machineItem->font(0);
            machineFont.setBold(true);
            machineItem->setFont(0, machineFont);

            // Load competencies for this machine
            QList<Competency> competencies = repository_.findCompetenciesByMachine(machine.id());
            for (const Competency& competency : competencies) {
                QTreeWidgetItem* competencyItem = new QTreeWidgetItem(machineItem);
                competencyItem->setText(0, competency.name());
                competencyItem->setText(1, "Competency");
                competencyItem->setText(2, QString("Max Score: %1").arg(competency.maxScore()));
                competencyItem->setText(3, QString::number(competency.id()));
                competencyItem->setData(0, Qt::UserRole, CompetencyItem);
            }

            machineItem->setExpanded(true);
        }
    }
}

void ProductionAreasWidget::showAreaManagementDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Manage Production Areas");
    dialog.resize(500, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    QLabel* titleLabel = new QLabel("Production Areas", &dialog);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QListWidget* areaList = new QListWidget(&dialog);
    for (const ProductionArea& area : allAreas_) {
        QListWidgetItem* item = new QListWidgetItem(area.name());
        item->setData(Qt::UserRole, area.id());
        areaList->addItem(item);
    }
    mainLayout->addWidget(areaList);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* addAreaBtn = new QPushButton("Add Area", &dialog);
    QPushButton* editAreaBtn = new QPushButton("Edit Area", &dialog);
    QPushButton* deleteAreaBtn = new QPushButton("Delete Area", &dialog);
    QPushButton* closeBtn = new QPushButton("Close", &dialog);

    connect(addAreaBtn, &QPushButton::clicked, [&]() {
        showAreaDialog(nullptr);
        // Refresh list
        areaList->clear();
        allAreas_ = repository_.findAllAreas();
        for (const ProductionArea& area : allAreas_) {
            QListWidgetItem* item = new QListWidgetItem(area.name());
            item->setData(Qt::UserRole, area.id());
            areaList->addItem(item);
        }
    });

    connect(editAreaBtn, &QPushButton::clicked, [&]() {
        QListWidgetItem* current = areaList->currentItem();
        if (!current) {
            QMessageBox::warning(&dialog, "No Selection", "Please select an area to edit.");
            return;
        }
        int areaId = current->data(Qt::UserRole).toInt();
        ProductionArea area = repository_.findAreaById(areaId);
        if (area.id() > 0) {
            showAreaDialog(&area);
            // Refresh list
            areaList->clear();
            allAreas_ = repository_.findAllAreas();
            for (const ProductionArea& area : allAreas_) {
                QListWidgetItem* item = new QListWidgetItem(area.name());
                item->setData(Qt::UserRole, area.id());
                areaList->addItem(item);
            }
        }
    });

    connect(deleteAreaBtn, &QPushButton::clicked, [&]() {
        QListWidgetItem* current = areaList->currentItem();
        if (!current) {
            QMessageBox::warning(&dialog, "No Selection", "Please select an area to delete.");
            return;
        }

        QString areaName = current->text();
        QMessageBox::StandardButton reply = QMessageBox::question(
            &dialog,
            "Confirm Delete",
            QString("Are you sure you want to delete '%1'?\nThis will also delete all machines and competencies in this area.").arg(areaName),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            int areaId = current->data(Qt::UserRole).toInt();
            if (repository_.removeArea(areaId)) {
                QMessageBox::information(&dialog, "Success", "Area deleted successfully.");
                // Refresh list
                areaList->clear();
                allAreas_ = repository_.findAllAreas();
                for (const ProductionArea& area : allAreas_) {
                    QListWidgetItem* item = new QListWidgetItem(area.name());
                    item->setData(Qt::UserRole, area.id());
                    areaList->addItem(item);
                }
            } else {
                QMessageBox::critical(&dialog, "Error", "Failed to delete area: " + repository_.lastError());
            }
        }
    });

    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    buttonLayout->addWidget(addAreaBtn);
    buttonLayout->addWidget(editAreaBtn);
    buttonLayout->addWidget(deleteAreaBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeBtn);

    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        loadProductionAreas();
    }
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
            loadAreaFilter();
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
            loadAreaFilter();
        }
    }
}

void ProductionAreasWidget::onAddClicked()
{
    int currentAreaId = areaFilterCombo_->currentData().toInt();

    if (currentAreaId == -1) {
        QMessageBox::information(this, "Select Area",
            "Please select a specific production area from the dropdown to add machines or competencies.\n\n"
            "Use the 'Manage Areas' button to add new production areas.");
        return;
    }

    QTreeWidgetItem* currentItem = treeWidget_->currentItem();

    if (!currentItem) {
        // No selection - add machine to selected area
        showMachineDialog(currentAreaId, nullptr);
    } else {
        int itemType = currentItem->data(0, Qt::UserRole).toInt();

        if (itemType == AreaItem || itemType == MachineItem) {
            // Add competency to selected machine or first machine under area
            int machineId;
            if (itemType == MachineItem) {
                machineId = currentItem->text(3).toInt();
            } else {
                // Get first machine in area
                if (currentItem->childCount() > 0) {
                    machineId = currentItem->child(0)->text(3).toInt();
                } else {
                    QMessageBox::information(this, "No Machines",
                        "Please add a machine first before adding competencies.");
                    return;
                }
            }

            // Ask what to add
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Add Item");
            msgBox.setText("What would you like to add?");
            QPushButton* machineBtn = msgBox.addButton("Add Machine", QMessageBox::ActionRole);
            QPushButton* competencyBtn = msgBox.addButton("Add Competency", QMessageBox::ActionRole);
            msgBox.addButton(QMessageBox::Cancel);

            msgBox.exec();

            if (msgBox.clickedButton() == machineBtn) {
                showMachineDialog(currentAreaId, nullptr);
            } else if (msgBox.clickedButton() == competencyBtn) {
                if (itemType == MachineItem) {
                    showCompetencyDialog(machineId, nullptr);
                } else {
                    QMessageBox::information(this, "Select Machine",
                        "Please select a machine to add a competency to it.");
                }
            }
        } else {
            QMessageBox::information(this, "Info", "Cannot add items to a competency. Select a machine to add a competency.");
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
    int id = currentItem->text(3).toInt();

    if (itemType == AreaItem) {
        ProductionArea area = repository_.findAreaById(id);
        if (area.id() > 0) {
            showAreaDialog(&area);
            loadProductionAreas();
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
    int id = currentItem->text(3).toInt();
    QString name = currentItem->text(0);

    QString itemTypeName;
    if (itemType == AreaItem) itemTypeName = "production area";
    else if (itemType == MachineItem) itemTypeName = "machine";
    else itemTypeName = "competency";

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete this %1: '%2'?\n%3")
            .arg(itemTypeName)
            .arg(name)
            .arg(itemType != CompetencyItem ? "This will also delete all child items." : ""),
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
            QMessageBox::information(this, "Success", QString("%1 deleted successfully.").arg(QString(itemTypeName[0].toUpper()) + itemTypeName.mid(1)));
            if (itemType == AreaItem) {
                loadProductionAreas();
            } else {
                loadAreaFilter();
            }
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
    int id = item->text(3).toInt();

    if (itemType == AreaItem) {
        ProductionArea area = repository_.findAreaById(id);
        if (area.id() > 0) {
            showAreaDialog(&area);
            loadProductionAreas();
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
