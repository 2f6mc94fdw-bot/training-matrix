# Building C++ Skill Matrix on macOS

## System Requirements
- **macOS**: 11 (Big Sur) or later
- **Xcode Command Line Tools**: Required
- **Homebrew**: Package manager (recommended)

---

## Quick Start (5 Minutes)

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Qt 6 and CMake
brew install qt@6 cmake

# Build the application
cd /path/to/training-matrix/cpp-skill-matrix
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./SkillMatrix.app/Contents/MacOS/SkillMatrix
```

---

## Detailed Installation

### Step 1: Install Xcode Command Line Tools

```bash
# Install command line tools
xcode-select --install

# Verify installation
xcode-select -p
# Should output: /Library/Developer/CommandLineTools
```

### Step 2: Install Homebrew (If Not Already Installed)

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add to PATH (for Apple Silicon Macs)
echo 'export PATH="/opt/homebrew/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# Verify
brew --version
```

### Step 3: Install Qt 6 via Homebrew

```bash
# Install Qt 6
brew install qt@6

# Add Qt to PATH
echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
echo 'export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6:$CMAKE_PREFIX_PATH"' >> ~/.zshrc
source ~/.zshrc

# Verify Qt installation
qmake6 --version
# Should output: QMake version 3.1, Using Qt version 6.x.x
```

### Step 4: Install CMake

```bash
# Install CMake
brew install cmake

# Verify
cmake --version
# Should be 3.20 or higher
```

---

## IDE Options for macOS

### Option 1: Visual Studio Code (Recommended)

**Why VS Code?**
- Excellent C++ and CMake support
- Free and lightweight
- Great Qt debugging integration
- Cross-platform

**Installation:**
```bash
brew install --cask visual-studio-code
```

**Setup:**
1. Install VS Code extensions:
   - **C/C++** (Microsoft) - IntelliSense, debugging
   - **CMake Tools** - CMake integration
   - **CMake Language Support** - Syntax highlighting
   - **Qt Tools** (optional) - Qt snippets

2. Open project:
   ```bash
   code /path/to/training-matrix/cpp-skill-matrix
   ```

3. Configure CMake:
   - Press `Cmd+Shift+P`
   - Type: "CMake: Configure"
   - Select kit: **Clang** (from Xcode)

4. Build:
   - Press `Cmd+Shift+B` or
   - Click "Build" in status bar or
   - Press `F7`

5. Run:
   - Press `Cmd+Shift+P`
   - Type: "CMake: Run Without Debugging"

**VS Code Settings (`.vscode/settings.json`):**
```json
{
  "cmake.configureSettings": {
    "CMAKE_PREFIX_PATH": "/opt/homebrew/opt/qt@6"
  },
  "cmake.generator": "Unix Makefiles",
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

---

### Option 2: Qt Creator (Best Qt Integration)

**Why Qt Creator?**
- Official Qt IDE
- Built-in Qt Designer
- Excellent Qt debugging
- Form editor integration

**Installation:**

**Method A: Via Qt Installer (Recommended)**
```bash
# Download Qt installer
curl -L -o ~/Downloads/qt-installer.dmg \
  https://download.qt.io/official_releases/online_installers/qt-unified-macOS-x64-online.dmg

# Open installer
open ~/Downloads/qt-installer.dmg
```

During installation select:
- ✅ Qt 6.8.0 (or latest)
- ✅ macOS
- ✅ Qt Creator
- ✅ Qt Charts
- ✅ Sources (optional)

**Method B: Via Homebrew**
```bash
brew install --cask qt-creator
```

**Open Project in Qt Creator:**
1. Launch Qt Creator
2. **File** → **Open File or Project**
3. Navigate to `cpp-skill-matrix/CMakeLists.txt`
4. Select kit: **Desktop Qt 6.x.x clang 64bit**
5. Click **Configure Project**
6. Press **Cmd+R** to build and run

---

### Option 3: Xcode

**Why Xcode?**
- Native macOS IDE
- Best macOS debugging tools
- Instruments integration

**Installation:**
```bash
# Install full Xcode (from App Store) or use command line tools
xcode-select --install
```

**Generate Xcode Project:**
```bash
cd /path/to/training-matrix/cpp-skill-matrix
mkdir build && cd build
cmake .. -G Xcode -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
```

**Open in Xcode:**
```bash
open SkillMatrix.xcodeproj
```

**Build and Run:**
1. Select **SkillMatrix** scheme
2. Press **Cmd+B** to build
3. Press **Cmd+R** to run

---

### Option 4: CLion (JetBrains)

**Why CLion?**
- Powerful C++ IDE
- Excellent CMake integration
- Great refactoring tools

**Installation:**
```bash
brew install --cask clion
```

**Setup:**
1. Open **CLion**
2. **Open** → Select `cpp-skill-matrix` folder
3. CLion detects `CMakeLists.txt` automatically
4. Set CMake options: **Settings** → **Build, Execution, Deployment** → **CMake**
   - Add: `-DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6`
5. Click **Reload CMake Project**
6. Press **Ctrl+R** to build and run

---

## Building from Command Line

### Standard Build

```bash
cd /path/to/training-matrix/cpp-skill-matrix
mkdir build && cd build

# Configure
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_BUILD_TYPE=Release

# Build (use all CPU cores)
cmake --build . -j$(sysctl -n hw.ncpu)

# Or build with make
make -j$(sysctl -n hw.ncpu)
```

### Debug Build

```bash
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(sysctl -n hw.ncpu)
```

### Clean Build

```bash
# Remove build directory
rm -rf build

# Rebuild from scratch
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build . -j$(sysctl -n hw.ncpu)
```

---

## Running the Application

### From Command Line

```bash
# Run from build directory
./SkillMatrix.app/Contents/MacOS/SkillMatrix

# Or use open command (launches as GUI app)
open SkillMatrix.app
```

### From Finder

Navigate to `cpp-skill-matrix/build/` and double-click `SkillMatrix.app`

---

## Database Setup (SQL Server)

### Option 1: SQL Server for Linux on Docker (Recommended)

```bash
# Install Docker Desktop for Mac
brew install --cask docker

# Run SQL Server in Docker
docker run -e "ACCEPT_EULA=Y" -e "MSSQL_SA_PASSWORD=YourStrong@Passw0rd" \
   -p 1433:1433 --name sqlserver \
   -d mcr.microsoft.com/mssql/server:2022-latest

# Connect to SQL Server
docker exec -it sqlserver /opt/mssql-tools/bin/sqlcmd \
   -S localhost -U SA -P "YourStrong@Passw0rd"
```

### Option 2: Azure SQL Database

Use your existing Azure SQL Database from the web app.

### Install ODBC Driver

```bash
# Install unixODBC
brew install unixodbc

# Install Microsoft ODBC Driver 18 for SQL Server
brew tap microsoft/mssql-release https://github.com/Microsoft/homebrew-mssql-release
brew update
brew install msodbcsql18 mssql-tools18

# Verify installation
odbcinst -j
ls /opt/homebrew/etc/odbcinst.ini
```

---

## Configuration

### Create Config File

```bash
# Create config directory
mkdir -p ~/.skillmatrix

# Create config.json
cat > ~/.skillmatrix/config.json << 'EOF'
{
  "database": {
    "server": "localhost",
    "database": "SkillMatrixDB",
    "username": "sa",
    "password": "YourStrong@Passw0rd",
    "port": 1433
  },
  "application": {
    "theme": "light",
    "sessionTimeout": 3600,
    "logLevel": "Info"
  }
}
EOF
```

### Load Database Schema

```bash
# If using Docker SQL Server
docker exec -it sqlserver /opt/mssql-tools/bin/sqlcmd \
   -S localhost -U SA -P "YourStrong@Passw0rd" \
   -i /path/to/cpp-skill-matrix/resources/database/schema.sql

# Or use Azure Data Studio (recommended GUI)
brew install --cask azure-data-studio
```

### Create Admin User

```sql
-- Connect to database and run:
INSERT INTO users (id, username, password, role, created_at, updated_at)
VALUES (
    'usr_' + CAST(DATEDIFF(SECOND, '1970-01-01', GETDATE()) AS VARCHAR(13)),
    'admin',
    '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9',
    'Admin',
    GETDATE(),
    GETDATE()
);
```

**Login credentials:**
- Username: `admin`
- Password: `admin123`

---

## Troubleshooting

### Issue: CMake cannot find Qt6

**Error:**
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solution:**
```bash
# Find Qt installation
brew --prefix qt@6
# Output: /opt/homebrew/opt/qt@6 (Apple Silicon)
#     or: /usr/local/opt/qt@6 (Intel Mac)

# Set CMAKE_PREFIX_PATH
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
```

### Issue: Qt DLLs not found at runtime

**Error:**
```
dyld: Library not loaded: @rpath/QtCore.framework/Versions/6/QtCore
```

**Solution:**
```bash
# Add Qt lib to DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$(brew --prefix qt@6)/lib:$DYLD_LIBRARY_PATH

# Or use macdeployqt to bundle frameworks
$(brew --prefix qt@6)/bin/macdeployqt SkillMatrix.app
```

### Issue: ODBC driver not found

**Error:**
```
QODBC driver not loaded
```

**Solution:**
```bash
# Check ODBC drivers
odbcinst -q -d

# Should see: [ODBC Driver 18 for SQL Server]

# If not, reinstall
brew reinstall msodbcsql18

# Check Qt SQL plugins
ls $(brew --prefix qt@6)/plugins/sqldrivers/
# Should see: libqsqlodbc.dylib
```

### Issue: Permission denied when running app

**Error:**
```
zsh: permission denied: ./SkillMatrix.app/Contents/MacOS/SkillMatrix
```

**Solution:**
```bash
chmod +x SkillMatrix.app/Contents/MacOS/SkillMatrix
```

### Issue: Architecture mismatch (Apple Silicon vs Intel)

**Error:**
```
Mach-O file is not a supported architecture
```

**Solution:**
```bash
# Check your Mac architecture
uname -m
# arm64 = Apple Silicon (M1/M2/M3)
# x86_64 = Intel

# Rebuild with correct architecture
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_OSX_ARCHITECTURES=$(uname -m)
cmake --build . -j$(sysctl -n hw.ncpu)
```

---

## Deployment

### Create Standalone App Bundle

```bash
# Use macdeployqt to bundle all Qt frameworks
$(brew --prefix qt@6)/bin/macdeployqt SkillMatrix.app

# Result: Self-contained app with all dependencies
# Can be copied to /Applications or distributed
```

### Create DMG Installer

```bash
# Install create-dmg
brew install create-dmg

# Create DMG
create-dmg \
  --volname "Skill Matrix Installer" \
  --window-pos 200 120 \
  --window-size 600 400 \
  --icon-size 100 \
  --icon "SkillMatrix.app" 175 120 \
  --hide-extension "SkillMatrix.app" \
  --app-drop-link 425 120 \
  "SkillMatrix-1.0.0.dmg" \
  "SkillMatrix.app"
```

---

## Performance Tips

### Build Performance
```bash
# Use Ninja generator (faster than Unix Makefiles)
brew install ninja
cmake .. -G Ninja -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
ninja

# Use ccache (caching compiler)
brew install ccache
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Runtime Performance
- Build with **Release** configuration for production
- Use **Debug** only during development
- Enable **Address Sanitizer** for debugging memory issues:
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=Debug \
           -DCMAKE_CXX_FLAGS="-fsanitize=address"
  ```

---

## Visual Studio for Mac Note

**Visual Studio for Mac** (being discontinued in 2024) has limited C++/Qt support. It's primarily for .NET/Xamarin development. For C++ Qt development on Mac, use:
1. **Visual Studio Code** (recommended)
2. **Qt Creator** (best Qt support)
3. **Xcode** (native macOS)
4. **CLion** (powerful but paid)

---

## IDE Comparison

| Feature | VS Code | Qt Creator | Xcode | CLion |
|---------|---------|------------|-------|-------|
| **Cost** | Free | Free | Free | Paid |
| **Qt Support** | Good | Excellent | Fair | Good |
| **CMake** | Excellent | Good | Fair | Excellent |
| **Debugging** | Good | Good | Excellent | Excellent |
| **Qt Designer** | No | Yes | No | Plugin |
| **Learning Curve** | Easy | Easy | Medium | Medium |

---

## Next Steps

1. ✅ Choose your IDE (VS Code recommended)
2. ✅ Install Qt 6 via Homebrew
3. ✅ Build the application
4. ✅ Set up SQL Server (Docker or Azure)
5. ✅ Configure database connection
6. ✅ Run and test the application

---

## Useful Commands

```bash
# Check system info
system_profiler SPSoftwareDataType SPHardwareDataType

# Check Qt version
qmake6 --version

# Check available Qt modules
ls $(brew --prefix qt@6)/lib/cmake/

# View app logs
tail -f ~/Library/Application\ Support/SkillMatrix/logs/app.log

# Monitor SQL Server (Docker)
docker logs -f sqlserver

# Clean build artifacts
rm -rf build
```

---

## Additional Resources

- **Qt for macOS**: https://doc.qt.io/qt-6/macos.html
- **Homebrew Qt**: https://formulae.brew.sh/formula/qt@6
- **VS Code C++**: https://code.visualstudio.com/docs/cpp/cpp-debug
- **Qt Creator**: https://www.qt.io/product/development-tools
- **ODBC on macOS**: https://docs.microsoft.com/en-us/sql/connect/odbc/linux-mac/

---

**Last Updated**: 2025-12-11
**Platform**: macOS 11+ (Intel & Apple Silicon)
**Recommended IDE**: Visual Studio Code or Qt Creator
**Qt Version**: 6.5+ via Homebrew
