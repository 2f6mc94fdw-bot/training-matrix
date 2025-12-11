# Build and Testing Guide - C++ Skill Matrix

This guide will help you build and test the C++ Skill Matrix application.

## Prerequisites

### Required Software

1. **Qt 6.5 or later**
   - Download from: https://www.qt.io/download-qt-installer
   - Required Qt modules:
     - Qt Core
     - Qt Widgets
     - Qt SQL
     - Qt Charts
     - Qt PrintSupport
     - Qt Network

2. **CMake 3.20 or later**
   - Download from: https://cmake.org/download/
   - Or install via package manager

3. **C++ Compiler**
   - **Windows**: Visual Studio 2019 or later (MSVC)
   - **Linux**: GCC 9+ or Clang 10+
   - **macOS**: Xcode 12+ (Apple Clang)

4. **Microsoft SQL Server**
   - SQL Server 2016 or later
   - SQL Server Express (free) is sufficient
   - Download from: https://www.microsoft.com/en-us/sql-server/sql-server-downloads

5. **ODBC Driver for SQL Server**
   - **Windows**: Usually included with SQL Server
   - **Linux**: Install `msodbcsql17` or `msodbcsql18`
   - **macOS**: Install via Homebrew: `brew install msodbcsql17`

### Optional Tools

- **Qt Creator** - Recommended IDE for Qt development
- **Visual Studio** (Windows) - Alternative IDE with good Qt integration
- **SQL Server Management Studio (SSMS)** - For database management

---

## Step 1: Install Qt

### Windows

1. Download Qt Online Installer from qt.io
2. Run installer and select:
   - Qt 6.5.0 (or later)
   - MSVC 2019 64-bit compiler
   - Qt Charts
   - Qt Creator (optional but recommended)
3. Note the installation path (e.g., `C:\Qt\6.5.0\msvc2019_64`)

### Linux (Ubuntu/Debian)

```bash
# Install Qt via apt
sudo apt update
sudo apt install qt6-base-dev qt6-charts-dev cmake build-essential

# Or download from Qt website for latest version
```

### macOS

```bash
# Install via Homebrew
brew install qt@6 cmake

# Or download from Qt website
```

---

## Step 2: Setup SQL Server Database

### Install SQL Server (if not already installed)

**Windows:**
1. Download SQL Server Express from Microsoft
2. Install with default settings
3. Note the server name (usually `localhost` or `.\SQLEXPRESS`)

**Linux:**
```bash
# Install SQL Server on Ubuntu
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
sudo add-apt-repository "$(wget -qO- https://packages.microsoft.com/config/ubuntu/20.04/mssql-server-2019.list)"
sudo apt update
sudo apt install -y mssql-server
sudo /opt/mssql/bin/mssql-conf setup
```

### Create Database

1. Open SQL Server Management Studio (SSMS) or use sqlcmd:

```sql
CREATE DATABASE training_matrix;
GO
```

2. Run the schema script:

```bash
# Using sqlcmd
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i resources/database/schema.sql

# Or execute in SSMS: File -> Open -> resources/database/schema.sql -> Execute
```

### Create Default Admin User

The schema already creates a default admin user. If needed, you can verify:

```sql
USE training_matrix;
SELECT * FROM users WHERE username = 'admin';
```

Default credentials:
- Username: `admin`
- Password: `admin123` (hashed in database)

---

## Step 3: Build the Application

### Option A: Using Qt Creator (Recommended)

1. **Open Project:**
   - Launch Qt Creator
   - File → Open File or Project
   - Navigate to `cpp-skill-matrix/CMakeLists.txt`
   - Click Open

2. **Configure Project:**
   - Qt Creator will detect your Qt installation
   - Select the Qt 6.x kit
   - Choose build configuration (Debug or Release)
   - Click "Configure Project"

3. **Build:**
   - Click the hammer icon (Build) or press Ctrl+B
   - Wait for compilation to complete

4. **Run:**
   - Click the play icon (Run) or press Ctrl+R
   - Application should launch

### Option B: Using Command Line

#### Windows (with MSVC)

```powershell
# Navigate to project directory
cd cpp-skill-matrix

# Create build directory
mkdir build
cd build

# Configure with CMake (adjust Qt path)
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64" -G "Visual Studio 16 2019"

# Build
cmake --build . --config Release

# Run
.\Release\SkillMatrix.exe
```

#### Linux

```bash
# Navigate to project directory
cd cpp-skill-matrix

# Create build directory
mkdir build && cd build

# Configure with CMake (adjust Qt path if needed)
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/qt6
# Or if Qt installed manually:
# cmake .. -DCMAKE_PREFIX_PATH=/opt/Qt/6.5.0/gcc_64

# Build
cmake --build . -j$(nproc)

# Run
./SkillMatrix
```

#### macOS

```bash
# Navigate to project directory
cd cpp-skill-matrix

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
# Or if Qt installed manually:
# cmake .. -DCMAKE_PREFIX_PATH=~/Qt/6.5.0/macos

# Build
cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./SkillMatrix.app/Contents/MacOS/SkillMatrix
```

---

## Step 4: Configure Database Connection

### First Run

When you first run the application, the login dialog will appear. Before logging in:

1. The application will try to connect to the database using default settings
2. If connection fails, you'll see an error message
3. You need to configure the database connection

### Configuration File

The application stores configuration in: `~/.skillmatrix/config.json` (or platform-specific location)

Create or edit the config file:

```json
{
  "database": {
    "server": "localhost",
    "database": "training_matrix",
    "user": "sa",
    "password": "YourSQLServerPassword",
    "port": 1433,
    "encrypt": true,
    "trustServerCertificate": true
  }
}
```

**Important:** Replace `YourSQLServerPassword` with your actual SQL Server SA password.

### Connection String Formats

**Local SQL Server:**
```json
"server": "localhost"
```

**Named Instance:**
```json
"server": "localhost\\SQLEXPRESS"
```

**Remote Server:**
```json
"server": "192.168.1.100"
```

---

## Step 5: Testing the Application

### Test 1: Login

1. Launch the application
2. Login dialog should appear
3. Enter credentials:
   - Username: `admin`
   - Password: `admin123`
4. Click "Login"
5. ✅ **Expected:** Main window appears with navigation sidebar

### Test 2: Navigation

1. After login, main window should show
2. Navigation sidebar on the left with menu items:
   - Dashboard
   - Engineers
   - Production Areas
   - Assessments
   - Core Skills
   - Reports
   - Analytics
   - Certifications
   - Snapshots
   - Audit Log
   - Import/Export
3. Click different menu items
4. ✅ **Expected:** Content area changes (shows "Coming Soon" placeholders)

### Test 3: Database Connectivity

1. Check logs in: `~/.local/share/SkillMatrix/skillmatrix_YYYY-MM-DD.log`
2. Look for:
   - "Successfully connected to database"
   - "Authentication successful: admin"
3. ✅ **Expected:** No database errors in logs

### Test 4: Engineers Query (if data exists)

1. Add test engineer to database:

```sql
USE training_matrix;
INSERT INTO engineers (id, name, shift, created_at, updated_at)
VALUES ('test_eng_1', 'John Doe', 'A Shift', GETDATE(), GETDATE());
```

2. In future: Engineers widget will display this data

---

## Troubleshooting

### Issue: Cannot find Qt

**Error:** `CMake Error: Could not find Qt6`

**Solution:**
```bash
# Specify Qt path explicitly
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/compiler
```

### Issue: ODBC Driver not found

**Error:** `Database connection failed: Driver not loaded`

**Solution:**
- **Windows:** Reinstall SQL Server client tools
- **Linux:**
  ```bash
  sudo apt install unixodbc unixodbc-dev
  curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
  curl https://packages.microsoft.com/config/ubuntu/20.04/prod.list | sudo tee /etc/apt/sources.list.d/mssql-release.list
  sudo apt update
  sudo ACCEPT_EULA=Y apt install -y msodbcsql17
  ```
- **macOS:**
  ```bash
  brew tap microsoft/mssql-release https://github.com/Microsoft/homebrew-mssql-release
  brew update
  brew install msodbcsql17
  ```

### Issue: Connection timeout

**Error:** `Database connection failed: Timeout`

**Solutions:**
1. Check SQL Server is running: `sc query MSSQLSERVER` (Windows)
2. Check firewall allows port 1433
3. Verify TCP/IP is enabled in SQL Server Configuration Manager
4. Test connection with sqlcmd:
   ```bash
   sqlcmd -S localhost -U sa -P YourPassword
   ```

### Issue: Login failed

**Error:** `Invalid username or password`

**Solutions:**
1. Verify admin user exists in database:
   ```sql
   SELECT * FROM users WHERE username = 'admin';
   ```
2. If password hash doesn't match, recreate admin user:
   ```sql
   -- Delete old admin
   DELETE FROM users WHERE username = 'admin';

   -- Create new admin (password hash for 'admin123')
   INSERT INTO users (id, username, password, role, created_at, updated_at)
   VALUES ('admin_id', 'admin',
           'hashed_password_here',  -- Use app's Crypto::hashPassword()
           'admin', GETDATE(), GETDATE());
   ```

### Issue: Compilation errors

**Common fixes:**
1. Ensure C++17 is supported:
   ```bash
   # Check compiler version
   g++ --version  # Should be 9+
   clang++ --version  # Should be 10+
   ```
2. Clean build directory:
   ```bash
   rm -rf build/
   mkdir build && cd build
   cmake ..
   ```
3. Update Qt to 6.5+

---

## Development Workflow

### Making Changes

1. **Edit code** in Qt Creator or your preferred editor
2. **Build** (Ctrl+B in Qt Creator)
3. **Run** (Ctrl+R in Qt Creator)
4. Check logs for errors

### Debugging

#### In Qt Creator:
1. Set breakpoints by clicking line numbers
2. Click Debug button (or F5)
3. Step through code with F10/F11

#### Via command line:
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Run with gdb (Linux/macOS)
gdb ./SkillMatrix

# Run with lldb (macOS)
lldb ./SkillMatrix
```

### Logging

Application logs are written to:
- **Windows:** `C:\Users\YourName\AppData\Local\SkillMatrix\skillmatrix_YYYY-MM-DD.log`
- **Linux:** `~/.local/share/SkillMatrix/skillmatrix_YYYY-MM-DD.log`
- **macOS:** `~/Library/Application Support/SkillMatrix/skillmatrix_YYYY-MM-DD.log`

Log levels:
- DEBUG: Detailed information
- INFO: General information
- WARNING: Warning messages
- ERROR: Error messages
- CRITICAL: Critical errors

---

## Next Steps

### Current Status ✅
- ✅ Application compiles
- ✅ Login works with database
- ✅ Navigation functional
- ✅ Database queries implemented

### To Implement 📝
- Engineers widget with table view
- Production areas tree view
- Assessment scoring interface
- Dashboard with statistics
- Reports and analytics
- Charts and visualization
- Import/Export functionality

---

## Getting Help

### Resources
- **Qt Documentation:** https://doc.qt.io/qt-6/
- **CMake Documentation:** https://cmake.org/documentation/
- **SQL Server Documentation:** https://docs.microsoft.com/en-us/sql/

### Common Commands

```bash
# Check Qt version
qmake --version

# Check CMake version
cmake --version

# Check SQL Server version
sqlcmd -S localhost -U sa -P YourPassword -Q "SELECT @@VERSION"

# View application logs (Linux/macOS)
tail -f ~/.local/share/SkillMatrix/skillmatrix_*.log

# Clean build
rm -rf build/ && mkdir build && cd build && cmake .. && cmake --build .
```

---

**Happy Building! 🚀**

For issues or questions, check the logs first, then refer to the troubleshooting section above.
