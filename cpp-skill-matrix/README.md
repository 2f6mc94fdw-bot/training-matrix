# Skill Matrix - C++ Desktop Application

A comprehensive cross-platform desktop application for training and competency management, built with Qt6 and C++.

## Features

### Core Functionality
- ✅ Engineer management with shift tracking
- ✅ Hierarchical production areas, machines, and competencies
- ✅ Assessment scoring system (0-3 scale)
- ✅ Core skills tracking across multiple categories
- ✅ User authentication with role-based access

### Reports & Analytics
- ✅ Interactive dashboard with statistics
- ✅ Skills gap analysis
- ✅ Automated training plan generation
- ✅ Visual heatmaps
- ✅ Individual engineer performance analysis
- ✅ Advanced analytics with predictions

### Advanced Features
- ✅ Certification tracking with expiry alerts
- ✅ Progress snapshots and historical comparison
- ✅ Complete audit trail
- ✅ Import/Export (Excel, JSON, CSV)
- ✅ Bulk operations
- ✅ Search and filtering

### User Experience
- ✅ Cross-platform (Windows, Linux, macOS)
- ✅ Light/Dark theme support
- ✅ Keyboard shortcuts
- ✅ Responsive UI
- ✅ Interactive charts and visualizations

## Technology Stack

- **Framework:** Qt 6.5+
- **Language:** C++17
- **Database:** Microsoft SQL Server
- **Build System:** CMake 3.20+
- **Charts:** Qt Charts module
- **Architecture:** Model-View-Controller (MVC)

## Prerequisites

### Required Software
1. **Qt 6.5 or later**
   - Download from: https://www.qt.io/download
   - Required modules: Core, Widgets, Sql, Charts, PrintSupport, Network

2. **CMake 3.20+**
   - Download from: https://cmake.org/download/

3. **C++ Compiler**
   - Windows: MSVC 2019+ (included with Visual Studio)
   - Linux: GCC 9+ or Clang 10+
   - macOS: Xcode 12+ (Apple Clang)

4. **Microsoft SQL Server**
   - SQL Server 2016 or later
   - SQL Server Express (free) is sufficient
   - ODBC Driver for SQL Server

### Optional Tools
- **Qt Creator** - Recommended IDE
- **Visual Studio** (Windows) - Alternative IDE
- **Git** - Version control

## Building from Source

### Using Qt Creator (Recommended)

1. **Open the project:**
   ```bash
   # Open CMakeLists.txt in Qt Creator
   ```

2. **Configure the project:**
   - Select your Qt kit (Qt 6.x with appropriate compiler)
   - Choose build configuration (Debug or Release)

3. **Build:**
   - Press Ctrl+B or click the hammer icon

4. **Run:**
   - Press Ctrl+R or click the play button

### Using Command Line

#### Linux/macOS

```bash
# Clone the repository
cd cpp-skill-matrix

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64

# Build
cmake --build .

# Run
./SkillMatrix
```

#### Windows

```powershell
# Clone the repository
cd cpp-skill-matrix

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.x\msvc2019_64"

# Build
cmake --build . --config Release

# Run
.\Release\SkillMatrix.exe
```

## Database Setup

1. **Create the database:**
   ```sql
   CREATE DATABASE training_matrix;
   GO
   ```

2. **Run the schema script:**
   - Open `resources/database/schema.sql` in SQL Server Management Studio
   - Execute the script (F5)

   Or via command line:
   ```bash
   sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i resources/database/schema.sql
   ```

3. **Configure connection:**
   - On first launch, the application will prompt for database credentials
   - Or edit the configuration file: `~/.skillmatrix/config.json`

### Example Configuration

```json
{
  "database": {
    "server": "localhost",
    "database": "training_matrix",
    "user": "sa",
    "password": "YourPassword123",
    "options": {
      "encrypt": true,
      "trustServerCertificate": true
    }
  }
}
```

## Default Login

- **Username:** `admin`
- **Password:** `admin123`

⚠️ **Change the default password immediately after first login!**

## Project Structure

```
cpp-skill-matrix/
├── CMakeLists.txt           # Build configuration
├── README.md               # This file
│
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── core/              # Core application classes
│   ├── models/            # Data models
│   ├── controllers/       # Business logic
│   ├── database/          # Data access layer
│   ├── ui/                # User interface
│   └── utils/             # Utilities
│
├── resources/             # Application resources
│   ├── icons/            # Application icons
│   ├── styles/           # Stylesheets (QSS)
│   └── database/         # SQL scripts
│
├── docs/                  # Documentation
│   ├── ARCHITECTURE.md   # Architecture guide
│   ├── DATABASE.md       # Database schema docs
│   └── USER_GUIDE.md     # User manual
│
├── tests/                 # Unit tests
└── build/                 # Build output (generated)
```

## Architecture

The application follows the **Model-View-Controller (MVC)** pattern:

- **Models** (`src/models/`) - Data structures and business objects
- **Views** (`src/ui/`) - User interface components
- **Controllers** (`src/controllers/`) - Business logic and coordination
- **Data Access Layer** (`src/database/`) - Database operations

### Key Design Patterns

- **Singleton:** DatabaseManager, Application
- **Repository:** Data access abstraction
- **Observer:** Qt signals/slots for event handling
- **Factory:** Object creation in repositories

## Usage

### Admin Functions
1. Manage production areas, machines, and competencies
2. Add/edit engineers and assign shifts
3. Create user accounts and manage permissions
4. Configure system settings

### Assessment Workflow
1. Select production area and machine
2. Choose engineer(s) to assess
3. Score competencies (0-3 scale)
4. Save assessments

### Generating Reports
1. Navigate to Reports section
2. Select report type (Dashboard, Skills Gap, Heatmap, etc.)
3. Apply filters as needed
4. Export to Excel or PDF

### Data Management
1. **Import:** File → Import → Select Excel/JSON file
2. **Export:** File → Export → Choose format
3. **Backup:** File → Create Snapshot

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+S` | Create snapshot |
| `Ctrl+E` | Export data |
| `Ctrl+I` | Import data |
| `Ctrl+K` | Global search |
| `Ctrl+F` | Find in current view |
| `Ctrl+N` | New item (context-dependent) |
| `Ctrl+Q` | Quit application |
| `F1` | Help |
| `F5` | Refresh |
| `F11` | Toggle fullscreen |

## Troubleshooting

### Cannot connect to SQL Server

1. Verify SQL Server is running
2. Check firewall settings (port 1433)
3. Verify credentials
4. Test connection:
   ```bash
   sqlcmd -S localhost -U sa -P YourPassword
   ```

### Application won't start

1. Verify Qt libraries are installed
2. Check Qt plugin path
3. Run from terminal to see error messages
4. Verify database connection settings

### Build errors

1. Ensure Qt 6.5+ is installed
2. Verify CMake can find Qt:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt
   ```
3. Check compiler version (C++17 support required)

## Contributing

This is a proprietary application for internal use. Contact the development team for contribution guidelines.

## License

Copyright © 2025. All rights reserved.

## Support

For issues or questions:
- Create an issue in the issue tracker
- Contact the development team
- See documentation in `docs/`

## Changelog

### Version 1.0.0 (Initial Release)
- Complete feature parity with web application
- Cross-platform support (Windows, Linux, macOS)
- SQL Server integration
- Full import/export capabilities
- Advanced analytics and reporting
