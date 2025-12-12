# Developer Quick Reference Guide

## Using StyleManager

### Initialize (in Application startup)
```cpp
StyleManager::instance().initialize();
```

### Apply Theme
```cpp
// Light theme
StyleManager::instance().applyTheme(StyleManager::Light);

// Dark theme
StyleManager::instance().applyTheme(StyleManager::Dark);
```

### Get Colors
```cpp
QColor primaryColor = StyleManager::instance().getColor("primary");
QColor dangerColor = StyleManager::instance().getColor("danger");
QColor textColor = StyleManager::instance().getColor("text");
```

### Apply to Widgets
```cpp
// Set stylesheet with color
myLabel->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");

// Use predefined button styles
QPushButton* btn = new QPushButton("Save", this);
btn->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));  // Blue
btn->setStyleSheet(StyleManager::instance().getButtonStyle("danger"));   // Red
btn->setStyleSheet(StyleManager::instance().getButtonStyle("success"));  // Green
btn->setStyleSheet(StyleManager::instance().getButtonStyle("secondary")); // Gray
```

### Available Colors
| Color Name | Light Theme | Dark Theme | Usage |
|------------|-------------|------------|-------|
| primary | #2196F3 | #42A5F5 | Primary actions, headers |
| secondary | #757575 | #90A4AE | Secondary actions |
| accent | #4CAF50 | #66BB6A | Success, highlights |
| danger | #F44336 | #EF5350 | Errors, delete actions |
| warning | #FF9800 | #FFA726 | Warnings, alerts |
| text | #212121 | #FFFFFF | Body text |
| textSecondary | #757575 | #B0B0B0 | Secondary text |
| background | #FAFAFA | #1E1E1E | Main background |
| border | #E0E0E0 | #424242 | Borders, dividers |

---

## Using IconProvider

### Initialize (in Application startup)
```cpp
IconProvider::instance().initialize();
```

### Get Icons
```cpp
// By type enum
QIcon addIcon = IconProvider::instance().getIcon(IconProvider::Add);
QIcon editIcon = IconProvider::instance().getIcon(IconProvider::Edit);
QIcon deleteIcon = IconProvider::instance().getIcon(IconProvider::Delete);

// By name string
QIcon saveIcon = IconProvider::instance().getIconByName("save");
```

### Use in Widgets
```cpp
// Buttons
QPushButton* btn = new QPushButton(
    IconProvider::instance().getIcon(IconProvider::Save),
    "Save",
    this
);

// List items
QListWidgetItem* item = new QListWidgetItem(
    IconProvider::instance().getIcon(IconProvider::Engineer),
    "John Smith"
);

// Tab widgets
tabWidget->addTab(
    widget,
    IconProvider::instance().getIcon(IconProvider::Dashboard),
    "Dashboard"
);
```

### Available Icon Types

**Actions:**
- Add, Edit, Delete, Save, Cancel
- Refresh, Search, Filter
- Export, Import, Print

**Data:**
- Engineer, ProductionArea, Machine
- Competency, Assessment, Certification, CoreSkill

**Status:**
- Success, Warning, Error, Info, Help

**UI:**
- Dashboard, Reports, Analytics
- Settings, Logout, Snapshot, AuditLog

**Misc:**
- Calendar, Chart, Document, Folder, File
- User, Users, Lock, Unlock

### Create Custom Icons
```cpp
// Colored square/circle icon
QIcon colorIcon = IconProvider::instance().createColorIcon(QColor("#4CAF50"), 16);

// Shape icon
QIcon circleIcon = IconProvider::instance().createShapeIcon("circle", QColor("#2196F3"), 20);
QIcon squareIcon = IconProvider::instance().createShapeIcon("square", QColor("#F44336"), 20);
QIcon triangleIcon = IconProvider::instance().createShapeIcon("triangle", QColor("#FF9800"), 20);
```

---

## Creating Modern Widgets

### Template Structure
```cpp
#include "StyleManager.h"
#include "../utils/IconProvider.h"

void MyWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    // Title with icon and color
    QLabel* titleLabel = new QLabel("📊 My Widget Title", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    mainLayout->addWidget(titleLabel);

    // Subtitle with secondary color
    QLabel* subtitleLabel = new QLabel("Widget description", this);
    subtitleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(10);

    // Card/GroupBox
    QGroupBox* cardBox = new QGroupBox("Card Title", this);
    QVBoxLayout* cardLayout = new QVBoxLayout(cardBox);

    // Card content here...

    mainLayout->addWidget(cardBox);

    // Buttons with icons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton* saveBtn = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Save),
        "Save",
        this
    );
    saveBtn->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));
    buttonLayout->addWidget(saveBtn);

    QPushButton* cancelBtn = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Cancel),
        "Cancel",
        this
    );
    cancelBtn->setStyleSheet(StyleManager::instance().getButtonStyle("secondary"));
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}
```

### Color-Coded Metric Cards
```cpp
QGroupBox* createMetricCard(const QString& title, const QString& emoji, const QString& value, const QString& subtitle, const QColor& color)
{
    QGroupBox* box = new QGroupBox(emoji + " " + title, this);
    QVBoxLayout* layout = new QVBoxLayout(box);

    QLabel* valueLabel = new QLabel(value, this);
    QFont countFont = valueLabel->font();
    countFont.setPointSize(36);
    countFont.setBold(true);
    valueLabel->setFont(countFont);
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet("color: " + color.name() + ";");
    layout->addWidget(valueLabel);

    QLabel* subtitleLabel = new QLabel(subtitle, this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    layout->addWidget(subtitleLabel);

    box->setMinimumHeight(140);
    return box;
}

// Usage
QGroupBox* engineersCard = createMetricCard(
    "Total Engineers",
    "👥",
    "25",
    "Active workforce",
    StyleManager::instance().getColor("primary")
);
```

---

## Working with QtCharts

### Include Headers
```cpp
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
```

### Create Bar Chart
```cpp
// Create chart
QChart* chart = new QChart();
chart->setTitle("Skill Distribution");
chart->setAnimationOptions(QChart::SeriesAnimations);

// Create bar set
QBarSet* set = new QBarSet("Assessments");
set->setColor(StyleManager::instance().getColor("primary"));
*set << 10 << 25 << 35 << 15;  // Data values

// Create series
QBarSeries* series = new QBarSeries();
series->append(set);
chart->addSeries(series);

// Setup axes
QStringList categories;
categories << "Not Assessed" << "Basic" << "Intermediate" << "Advanced";

QBarCategoryAxis* axisX = new QBarCategoryAxis();
axisX->append(categories);
chart->addAxis(axisX, Qt::AlignBottom);
series->attachAxis(axisX);

QValueAxis* axisY = new QValueAxis();
axisY->setTitleText("Count");
chart->addAxis(axisY, Qt::AlignLeft);
series->attachAxis(axisY);

// Create chart view
QChartView* chartView = new QChartView(chart, this);
chartView->setRenderHint(QPainter::Antialiasing);
chartView->setMinimumHeight(300);

// Add to layout
layout->addWidget(chartView);
```

---

## Generating Reports

### Basic Report Structure
```cpp
QString MyWidget::generateReport()
{
    QString report;
    QTextStream stream(&report);

    // Header
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "  REPORT TITLE\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP") << "\n";
    stream << "=" << QString("=").repeated(79) << "\n\n";

    // Content
    stream << "Total Items: " << items.size() << "\n\n";

    for (const Item& item : items) {
        stream << QString("-").repeated(79) << "\n";
        stream << "Item: " << item.name() << "\n";
        stream << "  Details: " << item.details() << "\n";
        stream << "\n";
    }

    // Footer
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "END OF REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";

    return report;
}
```

### Export to PDF
```cpp
bool exportToPDF(const QString& filename, const QString& content)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setPlainText(content);

    QFont font("Courier New", 8);
    doc.setDefaultFont(font);

    doc.print(&printer);

    return QFile::exists(filename);
}

// Usage
if (exportToPDF("/path/to/report.pdf", reportContent)) {
    QMessageBox::information(this, "Success", "Report exported!");
}
```

### Export to CSV
```cpp
bool exportToCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Header
    out << "Engineer ID,Name,Shift,Score\n";

    // Data
    for (const Engineer& engineer : engineers) {
        out << engineer.id() << ","
            << engineer.name() << ","
            << engineer.shift() << ","
            << engineer.score() << "\n";
    }

    file.close();
    return true;
}
```

---

## Best Practices

### 1. Always Use StyleManager for Colors
```cpp
// ✓ Good
label->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");

// ✗ Bad
label->setStyleSheet("color: #2196F3;");
```

### 2. Always Use IconProvider for Icons
```cpp
// ✓ Good
button->setIcon(IconProvider::instance().getIcon(IconProvider::Save));

// ✗ Bad
button->setIcon(QIcon(":/icons/save.png"));
```

### 3. Consistent Spacing
```cpp
mainLayout->setSpacing(16);  // Use 16px for main layouts
cardLayout->setSpacing(8);   // Use 8px for card internals
```

### 4. Color Coding for Metrics
- Primary (#2196F3): General/neutral metrics
- Accent/Success (#4CAF50): Positive/good metrics
- Warning (#FF9800): Attention/moderate metrics
- Danger (#F44336): Critical/error metrics

### 5. Proper Font Sizing
- Titles: 20-24pt, bold
- Subtitles: 12-14pt, normal
- Body: 10pt (default)
- Large numbers: 36pt, bold

### 6. Error Handling
```cpp
try {
    // Database operation
    QList<Engineer> engineers = engineerRepo_.findAll();

    if (engineers.isEmpty()) {
        Logger::instance().warning("MyWidget", "No engineers found");
        // Handle empty case
    }

} catch (const std::exception& e) {
    Logger::instance().error("MyWidget", QString("Error: %1").arg(e.what()));
    QMessageBox::critical(this, "Error", "Failed to load data");
}
```

### 7. Logging
```cpp
Logger::instance().info("MyWidget", "Operation completed successfully");
Logger::instance().warning("MyWidget", "Potential issue detected");
Logger::instance().error("MyWidget", "Critical error occurred");
Logger::instance().debug("MyWidget", "Debug information");
```

---

## Common Patterns

### Loading Data
```cpp
void MyWidget::loadData()
{
    try {
        // Fetch from repository
        QList<Item> items = itemRepo_.findAll();

        // Update UI
        updateDisplay(items);

        // Update timestamp
        lastUpdateLabel_->setText(
            "Last updated: " + QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP")
        );

        Logger::instance().info("MyWidget", QString("Loaded %1 items").arg(items.size()));

    } catch (const std::exception& e) {
        Logger::instance().error("MyWidget", QString("Failed to load data: %1").arg(e.what()));
        QMessageBox::critical(this, "Error", "Failed to load data from database");
    }
}
```

### Save with Confirmation
```cpp
void MyWidget::onSaveClicked()
{
    if (!validate()) {
        QMessageBox::warning(this, "Validation Error", "Please fix errors before saving");
        return;
    }

    try {
        if (itemRepo_.save(currentItem_)) {
            QMessageBox::information(this, "Success", "Saved successfully!");
            Logger::instance().info("MyWidget", "Item saved: " + currentItem_.id());
            emit itemSaved();
        } else {
            QMessageBox::critical(this, "Error", "Failed to save: " + itemRepo_.lastError());
        }
    } catch (const std::exception& e) {
        Logger::instance().error("MyWidget", QString("Save error: %1").arg(e.what()));
        QMessageBox::critical(this, "Error", "An error occurred while saving");
    }
}
```

### Delete with Confirmation
```cpp
void MyWidget::onDeleteClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        "Are you sure you want to delete this item?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        try {
            if (itemRepo_.remove(currentItem_.id())) {
                QMessageBox::information(this, "Success", "Deleted successfully!");
                Logger::instance().info("MyWidget", "Item deleted: " + currentItem_.id());
                loadData();  // Refresh
            } else {
                QMessageBox::critical(this, "Error", "Failed to delete");
            }
        } catch (const std::exception& e) {
            Logger::instance().error("MyWidget", QString("Delete error: %1").arg(e.what()));
            QMessageBox::critical(this, "Error", "An error occurred while deleting");
        }
    }
}
```

---

## Troubleshooting

### Colors Not Showing
```cpp
// Make sure StyleManager is initialized
StyleManager::instance().initialize();

// Apply theme
StyleManager::instance().applyTheme(StyleManager::Light);
```

### Icons Not Showing
```cpp
// Make sure IconProvider is initialized
IconProvider::instance().initialize();

// Check that QApplication is created before IconProvider::initialize()
```

### Charts Not Rendering
```cpp
// Include QtCharts in CMakeLists.txt
find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(${PROJECT_NAME} Qt6::Charts)

// Set render hints
chartView->setRenderHint(QPainter::Antialiasing);
```

### PDF Export Issues
```cpp
// Include PrintSupport in CMakeLists.txt
find_package(Qt6 REQUIRED COMPONENTS PrintSupport)
target_link_libraries(${PROJECT_NAME} Qt6::PrintSupport)

// Verify file was created
if (QFile::exists(filename)) {
    qDebug() << "PDF created successfully";
}
```

---

**End of Developer Guide**
