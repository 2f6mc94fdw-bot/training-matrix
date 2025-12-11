# Building C++ Skill Matrix on Windows with Visual Studio

## Prerequisites Already Installed ✅
- ✅ **Visual Studio** (2019 or 2022 recommended)
- ✅ **SQL Server** (from your existing web app)

## What You Need to Install

### 1. Install Qt 6 for Windows

#### Download Qt Online Installer
1. Go to: https://www.qt.io/download-qt-installer
2. Download: **Qt Online Installer for Windows**
3. Run the installer (requires Qt account - free to create)

#### Qt Installation Options
During installation, select:
- **Qt 6.8.0** (or latest 6.x version)
- **MSVC 2019 64-bit** or **MSVC 2022 64-bit** (match your Visual Studio version)
- ✅ Qt Charts
- ✅ Qt 5 Compatibility Module (optional)
- ✅ Qt Creator (optional - useful for Qt-specific debugging)

**Installation Path**: Typically `C:\Qt\6.8.0\msvc2022_64`

#### Add Qt to System PATH (Optional but Recommended)
1. Open **System Properties** → **Environment Variables**
2. Edit **Path** (user or system)
3. Add: `C:\Qt\6.8.0\msvc2022_64\bin`
4. Click OK

---

## Building the Application

### Method 1: Generate Visual Studio Solution (Recommended)

#### Step 1: Open Visual Studio Developer Command Prompt
- Open **Start Menu**
- Search: "Developer Command Prompt for VS 2022"
- Run as Administrator (recommended)

#### Step 2: Navigate to Project
```cmd
cd C:\path\to\training-matrix\cpp-skill-matrix
```

#### Step 3: Create Build Directory
```cmd
mkdir build
cd build
```

#### Step 4: Generate Visual Studio Solution
```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64
```

**For Visual Studio 2019:**
```cmd
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2019_64
```

This creates: `SkillMatrix.sln`

#### Step 5: Open in Visual Studio
```cmd
start SkillMatrix.sln
```

Or double-click `SkillMatrix.sln` in File Explorer

#### Step 6: Build the Application
In Visual Studio:
1. Select **Build Configuration**: Release (or Debug)
2. Right-click **SkillMatrix** project → **Set as Startup Project**
3. Press **F7** or **Build → Build Solution**
4. Press **F5** to run (or **Ctrl+F5** to run without debugging)

---

### Method 2: Build from Command Line

```cmd
cd C:\path\to\training-matrix\cpp-skill-matrix
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64
cmake --build . --config Release
```

Run the application:
```cmd
Release\SkillMatrix.exe
```

---

### Method 3: Open Folder in Visual Studio (CMake Integration)

Visual Studio 2019+ has built-in CMake support:

#### Step 1: Configure CMake
Create `CMakeSettings.json` in project root:
```json
{
  "configurations": [
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "buildRoot": "${projectDir}\\build\\${name}",
      "installRoot": "${projectDir}\\install\\${name}",
      "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "inheritEnvironments": [ "msvc_x64_x64" ]
    }
  ]
}
```

#### Step 2: Open Project
1. Visual Studio → **File** → **Open** → **Folder**
2. Select `cpp-skill-matrix` folder
3. Visual Studio will detect `CMakeLists.txt`
4. Wait for CMake configuration to complete

#### Step 3: Build and Run
- **Build**: **Build → Build All**
- **Run**: **Debug → Start Without Debugging** (Ctrl+F5)

---

## Installing ODBC Driver for SQL Server

### Check if Already Installed
1. Open **ODBC Data Sources (64-bit)**
2. Go to **Drivers** tab
3. Look for: "ODBC Driver 17 for SQL Server" or "ODBC Driver 18 for SQL Server"

### If Not Installed
1. Download: https://docs.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server
2. Install **ODBC Driver 18 for SQL Server**
3. Restart Visual Studio

---

## Database Configuration

### Step 1: Create Configuration File
Create: `C:\Users\YourUsername\.skillmatrix\config.json`

```json
{
  "database": {
    "server": "localhost",
    "database": "SkillMatrixDB",
    "username": "your_username",
    "password": "your_password",
    "port": 1433
  },
  "application": {
    "theme": "light",
    "sessionTimeout": 3600,
    "logLevel": "Info"
  }
}
```

### Step 2: Update Database Schema
Use your existing SQL Server database from the web app, or create tables using:
```cmd
sqlcmd -S localhost -d SkillMatrixDB -i ..\resources\database\schema.sql
```

### Step 3: Create Default Admin User
```sql
INSERT INTO users (id, username, password, role, created_at, updated_at)
VALUES (
    'usr_' + CONVERT(VARCHAR(13), DATEDIFF(SECOND, '1970-01-01', GETDATE())),
    'admin',
    -- Password: 'admin123' hashed with SHA-256 (10 rounds)
    '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9',
    'Admin',
    GETDATE(),
    GETDATE()
);
```

---

## Running the Application

### From Visual Studio
1. Press **F5** (Debug mode)
2. Or **Ctrl+F5** (Release mode without debugging)

### From File Explorer
Navigate to:
```
cpp-skill-matrix\build\Release\SkillMatrix.exe
```
Double-click to run.

### Login
- **Username**: `admin`
- **Password**: `admin123`

---

## Troubleshooting

### Issue: CMake cannot find Qt6

**Error:**
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solution:**
1. Verify Qt installation path: `C:\Qt\6.8.0\msvc2022_64`
2. Update CMAKE_PREFIX_PATH in cmake command:
   ```cmd
   cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64
   ```

### Issue: Missing Qt DLLs at Runtime

**Error:**
```
The program can't start because Qt6Core.dll is missing
```

**Solution:**
Copy Qt DLLs to executable directory, or add Qt bin to PATH:
```cmd
set PATH=C:\Qt\6.8.0\msvc2022_64\bin;%PATH%
```

**Or use `windeployqt` to copy all required DLLs:**
```cmd
cd build\Release
C:\Qt\6.8.0\msvc2022_64\bin\windeployqt.exe SkillMatrix.exe
```

### Issue: QODBC driver not loaded

**Error:**
```
QODBC driver not loaded
```

**Solution:**
1. Check ODBC drivers are installed (see above)
2. Verify `qsqlodbc.dll` exists in: `C:\Qt\6.8.0\msvc2022_64\plugins\sqldrivers\`
3. If missing, reinstall Qt with SQL plugins

### Issue: Cannot connect to SQL Server

**Error:**
```
Database connection failed
```

**Solution:**
1. Verify SQL Server is running:
   ```cmd
   sqlcmd -S localhost -U sa -P YourPassword
   ```
2. Enable TCP/IP in SQL Server Configuration Manager
3. Open port 1433 in Windows Firewall
4. Check connection string in config.json

### Issue: Build fails with "Qt6Charts not found"

**Error:**
```
CMake Error: Could not find a package configuration file provided by "Qt6Charts"
```

**Solution:**
1. Re-run Qt Maintenance Tool
2. Add: **Qt Charts** component
3. Re-run CMake configuration

---

## Project Structure in Visual Studio

```
SkillMatrix (Solution)
├── SkillMatrix (Project)
│   ├── Header Files
│   │   ├── core/
│   │   ├── database/
│   │   ├── models/
│   │   ├── controllers/
│   │   ├── ui/
│   │   └── utils/
│   ├── Source Files
│   │   ├── main.cpp
│   │   ├── core/
│   │   ├── database/
│   │   └── ...
│   └── Resource Files
│       └── resources.qrc
└── CMakeLists.txt
```

---

## Development Workflow

### Making Changes
1. Edit code in Visual Studio
2. Press **F7** to build
3. Press **F5** to run and debug
4. Set breakpoints by clicking left margin
5. Use **Watch** window to inspect variables

### Debugging
- **F9**: Toggle breakpoint
- **F5**: Start debugging
- **F10**: Step over
- **F11**: Step into
- **Shift+F5**: Stop debugging

### Qt Designer (Optional)
For editing `.ui` files:
1. Install Qt Creator (comes with Qt)
2. Open `.ui` files in Qt Designer
3. Save and rebuild in Visual Studio

---

## Building for Distribution

### Step 1: Build Release Version
```cmd
cmake --build . --config Release
```

### Step 2: Deploy Qt Dependencies
```cmd
cd build\Release
C:\Qt\6.8.0\msvc2022_64\bin\windeployqt.exe --release SkillMatrix.exe
```

This copies all required Qt DLLs, plugins, and resources.

### Step 3: Create Installer (Optional)
Use tools like:
- **Inno Setup** (free)
- **WiX Toolset** (free)
- **Qt Installer Framework**

---

## Performance Tips

### Optimize Build Time
- Use **Ninja** generator instead of Visual Studio:
  ```cmd
  cmake .. -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64
  ninja
  ```
- Enable **parallel builds**: Project Properties → C/C++ → General → Multi-processor Compilation

### Release vs Debug
- **Debug**: Full symbols, slower, easier debugging
- **Release**: Optimized, faster, smaller executable

---

## Next Steps

1. ✅ Install Qt 6 for MSVC
2. ✅ Generate Visual Studio solution
3. ✅ Build the application
4. ✅ Configure database connection
5. ✅ Run and test login
6. ✅ Explore the UI and features

---

## Useful Visual Studio Shortcuts

| Shortcut | Action |
|----------|--------|
| **F5** | Start debugging |
| **Ctrl+F5** | Run without debugging |
| **F7** | Build solution |
| **F9** | Toggle breakpoint |
| **F10** | Step over |
| **F11** | Step into |
| **Ctrl+K, Ctrl+D** | Format document |
| **Ctrl+.** | Quick actions |

---

## Additional Resources

- **Qt Documentation**: https://doc.qt.io/qt-6/
- **Qt for Windows**: https://doc.qt.io/qt-6/windows.html
- **Visual Studio CMake**: https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio
- **ODBC Driver**: https://docs.microsoft.com/en-us/sql/connect/odbc/windows/

---

**Last Updated**: 2025-12-11
**Platform**: Windows 10/11
**IDE**: Visual Studio 2019/2022
**Qt Version**: 6.5+ (MSVC build)
