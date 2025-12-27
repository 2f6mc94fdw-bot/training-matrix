# Excel Import/Export Utilities

## Status: Not Implemented

The Excel import and export functionality (`ExcelImporter` and `ExcelExporter` classes) are currently stubbed and require the **QXlsx** library for full implementation.

## Requirements

### QXlsx Library

QXlsx is a Qt-based library for reading and writing Excel files (.xlsx format).

**Installation:**

- **Ubuntu/Debian:**
  ```bash
  # QXlsx is not available in standard repositories
  # Build from source: https://github.com/QtExcel/QXlsx
  ```

- **macOS (Homebrew):**
  ```bash
  # Build from source: https://github.com/QtExcel/QXlsx
  ```

- **Windows:**
  ```bash
  # Build from source or use vcpkg
  vcpkg install qxlsx
  ```

### CMake Integration

After installing QXlsx, update `cpp-skill-matrix/CMakeLists.txt`:

```cmake
find_package(QXlsx REQUIRED)

target_link_libraries(skill_matrix_app
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    QXlsx::QXlsx  # Add this line
)
```

## Implementation Tasks

### ExcelImporter (4 methods to implement)

1. `importEngineers(filePath)` - Import engineer data from Excel
2. `importProductionAreas(filePath)` - Import production areas, machines, and competencies
3. `importAssessments(filePath)` - Import assessment records
4. `importAll(filePath)` - Comprehensive import from structured Excel file

**Expected Excel Format:**
- Sheet 1: Engineers (columns: ID, Name, Shift, Created At)
- Sheet 2: Production Areas (columns: ID, Name, Created At)
- Sheet 3: Machines (columns: ID, Name, Production Area ID, Created At)
- Sheet 4: Competencies (columns: ID, Machine ID, Name, Importance, Max Score)
- Sheet 5: Assessments (columns: Engineer ID, Competency ID, Score, Notes, Assessed At)

### ExcelExporter (5 methods to implement)

1. `exportAll(filePath)` - Export complete database to multi-sheet Excel file
2. `exportEngineers(filePath)` - Export engineers list
3. `exportProductionAreas(filePath)` - Export production hierarchy
4. `exportAssessments(filePath)` - Export all assessment records
5. `exportReport(filePath, reportType)` - Export formatted reports

**Export Features:**
- Multiple sheets for different data types
- Headers with column names
- Formatted cells (bold headers, borders)
- Date/time formatting
- Auto-column width adjustment

## Code Example

Once QXlsx is installed, implementation will look like:

```cpp
#include <xlsxdocument.h>

ImportResult ExcelImporter::importEngineers(const QString& filePath)
{
    ImportResult result;
    QXlsx::Document xlsx(filePath);

    if (!xlsx.load()) {
        result.errors << "Failed to open Excel file";
        return result;
    }

    // Read data from worksheet
    int row = 2; // Start after header
    while (true) {
        QVariant idCell = xlsx.read(row, 1);
        if (idCell.isNull()) break;

        QString id = idCell.toString();
        QString name = xlsx.read(row, 2).toString();
        QString shift = xlsx.read(row, 3).toString();

        // Save to database using EngineerController
        // ...

        result.recordsProcessed++;
        row++;
    }

    result.success = true;
    return result;
}
```

## Alternative Solutions

If QXlsx installation is problematic, consider:

1. **CSV Import/Export** - Simpler format, easier to implement with Qt's built-in classes
2. **Qt Commercial Charts** - Includes Excel export in commercial Qt license
3. **Manual XML** - .xlsx files are ZIP archives with XML; can be parsed manually
4. **Python Integration** - Use Python with openpyxl library via QProcess

## Testing

After implementation, test with:
- Small sample files (10-20 records)
- Large files (1000+ records)
- Files with missing data
- Files with invalid data types
- Export then re-import (roundtrip testing)

## Priority

**Phase 5 Feature** - Not required for core functionality. The application is fully functional for manual data entry and viewing. Excel import/export is a convenience feature for bulk operations.
