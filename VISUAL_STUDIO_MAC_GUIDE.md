# Building C++ Skill Matrix with Visual Studio for Mac

## Overview

Visual Studio for Mac can work with C++ CMake projects. Here's how to set up and build the Qt Skill Matrix application.

---

## Prerequisites

### 1. Install Qt 6 for macOS

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Qt 6 and CMake
brew install qt@6 cmake

# Verify installation
qmake6 --version
# Should show: QMake version 3.1 using Qt version 6.x.x

# Get Qt installation path (you'll need this)
brew --prefix qt@6
# Typically: /opt/homebrew/opt/qt@6 (Apple Silicon) or /usr/local/opt/qt@6 (Intel)
```

### 2. Install Xcode Command Line Tools

Visual Studio for Mac requires Xcode tools for C++ compilation:

```bash
xcode-select --install
```

---

## Opening the Project in Visual Studio for Mac

### Method 1: Open as CMake Project (Recommended)

Visual Studio for Mac 2022 (v17.0+) supports CMake projects:

#### Step 1: Open Project
1. Launch **Visual Studio for Mac**
2. Click **Open** (or File → Open)
3. Navigate to: `training-matrix/cpp-skill-matrix`
4. Select **CMakeLists.txt**
5. Click **Open**

#### Step 2: Configure CMake Settings
1. Visual Studio will detect it's a CMake project
2. Go to **Project → CMake Settings**
3. Add CMake configuration:
   - **Configuration**: Release
   - **CMake Arguments**: `-DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6`
   - **Build Directory**: `${projectDir}/build`

#### Step 3: Build
1. **Build → Build All** (⌘B)
2. Wait for compilation to complete

#### Step 4: Run
1. **Run → Start Without Debugging** (⌃F5)
2. Or configure run settings:
   - **Run → Edit Configurations**
   - Set executable: `build/SkillMatrix.app/Contents/MacOS/SkillMatrix`

---

### Method 2: Build from Terminal, Run in VS for Mac

If CMake integration has issues, build via Terminal:

#### Step 1: Build Project
```bash
cd ~/training-matrix/cpp-skill-matrix
mkdir -p build && cd build

# Configure with CMake
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j$(sysctl -n hw.ncpu)
```

#### Step 2: Open Executable in VS for Mac
1. In Visual Studio for Mac: **Run → Edit Configurations**
2. Add new **Custom Command**:
   - **Name**: SkillMatrix
   - **Executable**: `${ProjectDir}/build/SkillMatrix.app/Contents/MacOS/SkillMatrix`
   - **Working Directory**: `${ProjectDir}/build`
3. Click **OK**
4. Select configuration and press **F5** to debug

---

### Method 3: Terminal-Only Workflow

If Visual Studio for Mac has CMake issues, use Terminal for building:

```bash
# Navigate to project
cd ~/training-matrix/cpp-skill-matrix

# Create build directory
mkdir -p build && cd build

# Configure
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./SkillMatrix.app/Contents/MacOS/SkillMatrix
```

Then use Visual Studio for Mac for **editing code only**, rebuild from Terminal when needed.

---

## Configuration for Apple Silicon vs Intel

### Apple Silicon (M1/M2/M3)
```bash
# Qt path
CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6

# Architecture
-DCMAKE_OSX_ARCHITECTURES=arm64
```

### Intel Mac
```bash
# Qt path
CMAKE_PREFIX_PATH=/usr/local/opt/qt@6

# Architecture
-DCMAKE_OSX_ARCHITECTURES=x86_64
```

---

## Installing ODBC Driver for SQL Server on Mac

### Install Microsoft ODBC Driver

```bash
# Install ODBC driver via Homebrew
brew tap microsoft/mssql-release https://github.com/Microsoft/homebrew-mssql-release
brew update
brew install msodbcsql18 mssql-tools18

# Verify installation
odbcinst -q -d
# Should show: [ODBC Driver 18 for SQL Server]
```

### Configure ODBC

Check ODBC configuration:
```bash
cat /usr/local/etc/odbcinst.ini
```

Should contain:
```ini
[ODBC Driver 18 for SQL Server]
Description=Microsoft ODBC Driver 18 for SQL Server
Driver=/usr/local/lib/libmsodbcsql.18.dylib
UsageCount=1
```

---

## Database Configuration

Create configuration file at: `~/.skillmatrix/config.json`

```bash
mkdir -p ~/.skillmatrix
nano ~/.skillmatrix/config.json
```

Add:
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

---

## Project Structure in Visual Studio for Mac

```
cpp-skill-matrix/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── Application.h/cpp
│   │   ├── Session.h/cpp
│   │   └── Constants.h
│   ├── database/
│   │   ├── DatabaseManager.h/cpp
│   │   └── *Repository.h/cpp
│   ├── models/
│   │   └── *.h/cpp
│   ├── controllers/
│   ├── ui/
│   │   ├── MainWindow.h/cpp
│   │   ├── LoginDialog.h/cpp
│   │   └── widgets/
│   └── utils/
└── resources/
```

---

## Debugging in Visual Studio for Mac

### Set Breakpoints
1. Click in the **gutter** (left of line numbers) to set breakpoint
2. Red dot appears

### Start Debugging
1. **Run → Start Debugging** (F5)
2. Or **Run → Start Without Debugging** (⌃F5)

### Debug Controls
- **F10**: Step Over
- **F11**: Step Into
- **⇧F11**: Step Out
- **⌘K**: Continue
- **⇧F5**: Stop Debugging

### Watch Variables
1. While debugging, hover over variables
2. Use **Locals** window to inspect variables
3. Add watches in **Watch** window

---

## Troubleshooting

### Issue: CMake not found

**Error:**
```
CMake is not installed
```

**Solution:**
```bash
brew install cmake
# Restart Visual Studio for Mac
```

### Issue: Qt6 not found

**Error:**
```
Could not find a package configuration file provided by "Qt6"
```

**Solution:**
```bash
# Verify Qt is installed
brew list qt@6

# Get Qt path
brew --prefix qt@6

# Use full path in CMake arguments
-DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
```

### Issue: Cannot run executable

**Error:**
```
The application cannot be started
```

**Solution:**
Check that Qt libraries are in DYLD path:
```bash
export DYLD_LIBRARY_PATH=$(brew --prefix qt@6)/lib:$DYLD_LIBRARY_PATH
```

Or add to `~/.zshrc`:
```bash
echo 'export DYLD_LIBRARY_PATH=$(brew --prefix qt@6)/lib:$DYLD_LIBRARY_PATH' >> ~/.zshrc
source ~/.zshrc
```

### Issue: Visual Studio for Mac doesn't support CMake well

**Workaround:**
Use **Visual Studio Code** instead (different product):

```bash
# Install VS Code
brew install --cask visual-studio-code

# Install extensions
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools

# Open project
cd ~/training-matrix/cpp-skill-matrix
code .
```

VS Code has better CMake/C++ support than Visual Studio for Mac.

---

## Building for Distribution (macOS)

### Step 1: Build Release Version
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build . --config Release
```

### Step 2: Deploy Qt Frameworks
```bash
# Use macdeployqt to bundle Qt libraries
$(brew --prefix qt@6)/bin/macdeployqt SkillMatrix.app -dmg

# This creates: SkillMatrix.dmg (distributable)
```

### Step 3: Code Sign (Optional)
```bash
codesign --force --deep --sign - SkillMatrix.app
```

---

## Performance Tips

### Faster Builds
```bash
# Use Ninja generator (faster than Unix Makefiles)
brew install ninja
cmake .. -G Ninja -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
ninja
```

### Parallel Compilation
```bash
# Use all CPU cores
cmake --build . -j$(sysctl -n hw.ncpu)
```

---

## Quick Reference Commands

```bash
# Install dependencies
brew install qt@6 cmake

# Configure project
cd ~/training-matrix/cpp-skill-matrix
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)

# Build
cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./SkillMatrix.app/Contents/MacOS/SkillMatrix

# Clean build
rm -rf build && mkdir build && cd build

# Rebuild
cmake --build . --clean-first
```

---

## Alternative: Qt Creator (Better Qt Support)

If Visual Studio for Mac has issues with Qt, consider **Qt Creator**:

```bash
# Download Qt with Qt Creator
# Visit: https://www.qt.io/download-qt-installer
# Or install via Homebrew
brew install --cask qt-creator
```

Qt Creator has **native Qt support** and is often easier for Qt projects.

---

## Login Credentials

Once the app runs:
- **Username**: `admin`
- **Password**: `admin123`

---

## Next Steps

1. ✅ Install Qt 6 via Homebrew
2. ✅ Install ODBC driver for SQL Server
3. ✅ Open project in Visual Studio for Mac
4. ✅ Configure CMake with Qt path
5. ✅ Build and run
6. ✅ Test database connection and login

---

**Last Updated**: 2025-12-11
**Platform**: macOS (Apple Silicon & Intel)
**IDE**: Visual Studio for Mac 2022
**Qt Version**: 6.x (Homebrew)
