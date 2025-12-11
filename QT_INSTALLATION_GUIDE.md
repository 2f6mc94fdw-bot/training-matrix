# Qt 6 Installation Guide for C++ Skill Matrix

## Current System Information
- **OS**: Ubuntu 24.04.3 LTS (Noble Numbat)
- **CMake**: 3.28.3 ✅ (Already installed)
- **Qt Status**: Not installed ❌

---

## Installation Methods

### Method 1: Ubuntu Package Manager (Recommended - Easiest)

This method installs Qt 6.4.2 which is sufficient for the Skill Matrix application.

#### Step 1: Ensure network connectivity
```bash
# Test network connection
ping -c 3 archive.ubuntu.com
```

#### Step 2: Update package lists
```bash
sudo apt update
```

#### Step 3: Install Qt 6 and required modules
```bash
# Install Qt 6 base development packages
sudo apt install -y \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    libqt6sql6 \
    libqt6sql6-odbc \
    build-essential

# Install Qt Charts (required for analytics)
sudo apt install -y libqt6charts6-dev

# Install additional Qt modules
sudo apt install -y \
    libqt6core6 \
    libqt6gui6 \
    libqt6widgets6 \
    libqt6printsupport6 \
    libqt6network6
```

#### Step 4: Verify installation
```bash
# Check Qt version
qmake6 --version

# Should output: QMake version 3.1 using Qt version 6.4.2

# Check CMake can find Qt
cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=EXIST
```

#### Step 5: Find Qt installation path
```bash
# Find Qt6 cmake modules location
dpkg -L qt6-base-dev | grep Qt6Config.cmake

# Typical output: /usr/lib/x86_64-linux-gnu/cmake/Qt6/Qt6Config.cmake
# So the CMAKE_PREFIX_PATH would be: /usr/lib/x86_64-linux-gnu/cmake/Qt6
```

---

### Method 2: Qt Online Installer (Latest Qt 6.5+)

Use this method if you need Qt 6.5+ or want Qt Creator IDE.

#### Step 1: Download Qt Online Installer
```bash
# Download the installer
wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run

# Make it executable
chmod +x qt-unified-linux-x64-online.run
```

#### Step 2: Run the installer
```bash
# Run installer (requires GUI)
./qt-unified-linux-x64-online.run
```

#### Step 3: Installer Options
During installation, select:
- ✅ Qt 6.8.0 (or latest 6.x version)
- ✅ Desktop gcc 64-bit
- ✅ Qt Charts
- ✅ Qt Creator (optional but recommended)
- ✅ CMake integration

Installation location: `/opt/Qt/6.8.0/gcc_64` (or similar)

#### Step 4: Set up environment variables
```bash
# Add to ~/.bashrc or ~/.profile
echo 'export PATH=/opt/Qt/6.8.0/gcc_64/bin:$PATH' >> ~/.bashrc
echo 'export CMAKE_PREFIX_PATH=/opt/Qt/6.8.0/gcc_64' >> ~/.bashrc

# Reload
source ~/.bashrc
```

---

### Method 3: Manual Download (Offline Installation)

If you need to install without direct internet access:

#### Step 1: Download Qt packages manually
On a machine with internet access, download:
- Qt base development files
- Qt Charts
- Qt tools

From: https://download.qt.io/

#### Step 2: Transfer and install
Transfer .deb files to your Ubuntu system and install with:
```bash
sudo dpkg -i *.deb
sudo apt --fix-broken install
```

---

## Installing ODBC Driver for SQL Server

### Ubuntu 24.04

```bash
# Install dependencies
sudo apt install -y unixodbc unixodbc-dev

# Add Microsoft repository
curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
curl https://packages.microsoft.com/config/ubuntu/24.04/prod.list | \
    sudo tee /etc/apt/sources.list.d/mssql-release.list

# Update and install ODBC driver
sudo apt update
sudo ACCEPT_EULA=Y apt install -y msodbcsql18

# Verify installation
odbcinst -q -d -n "ODBC Driver 18 for SQL Server"
```

---

## Building the Skill Matrix Application

### Step 1: Navigate to project
```bash
cd /home/user/training-matrix/cpp-skill-matrix
```

### Step 2: Create build directory
```bash
mkdir -p build
cd build
```

### Step 3: Configure with CMake

**If using Ubuntu packages (Method 1):**
```bash
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

**If using Qt installer (Method 2):**
```bash
cmake .. -DCMAKE_PREFIX_PATH=/opt/Qt/6.8.0/gcc_64
```

**Or let CMake find Qt automatically:**
```bash
cmake ..
```

### Step 4: Build the application
```bash
# Build with all CPU cores
cmake --build . -j$(nproc)

# Or build with specific target
cmake --build . --target SkillMatrix
```

### Step 5: Run the application
```bash
./SkillMatrix
```

---

## Troubleshooting

### Issue: CMake cannot find Qt6

**Error:**
```
CMake Error at CMakeLists.txt:X (find_package):
  Could not find a package configuration file provided by "Qt6"
```

**Solution:**
```bash
# Find where Qt6 is installed
find /usr -name Qt6Config.cmake 2>/dev/null
find /opt -name Qt6Config.cmake 2>/dev/null

# Use the path in CMAKE_PREFIX_PATH
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6/cmake
```

### Issue: Missing Qt modules

**Error:**
```
Could not find a package configuration file provided by "Qt6" with any of the following names:
  Qt6ChartsConfig.cmake
```

**Solution:**
```bash
# Install missing module
sudo apt install -y libqt6charts6-dev

# Or with Qt installer, ensure Charts module is selected
```

### Issue: ODBC driver not found at runtime

**Error in application:**
```
Database connection failed: Driver not loaded
QODBC: unable to load driver
```

**Solution:**
```bash
# Check available ODBC drivers
odbcinst -q -d

# If "ODBC Driver 18 for SQL Server" is not listed, install it:
sudo ACCEPT_EULA=Y apt install -y msodbcsql18

# List Qt SQL drivers
ls /usr/lib/x86_64-linux-gnu/qt6/plugins/sqldrivers/
# Should see: libqsqlodbc.so
```

### Issue: Application won't start - missing libraries

**Error:**
```
error while loading shared libraries: libQt6Core.so.6: cannot open shared object file
```

**Solution:**
```bash
# Add Qt lib path to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

# Or for Qt installer:
export LD_LIBRARY_PATH=/opt/Qt/6.8.0/gcc_64/lib:$LD_LIBRARY_PATH

# Make permanent by adding to ~/.bashrc
```

---

## Quick Start Commands (Copy & Paste)

Once network is available, run these commands to install everything:

```bash
# Update system
sudo apt update

# Install Qt 6 and all dependencies
sudo apt install -y \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    libqt6charts6-dev \
    libqt6sql6 \
    libqt6sql6-odbc \
    build-essential \
    unixodbc \
    unixodbc-dev

# Install ODBC Driver for SQL Server
curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
curl https://packages.microsoft.com/config/ubuntu/24.04/prod.list | \
    sudo tee /etc/apt/sources.list.d/mssql-release.list
sudo apt update
sudo ACCEPT_EULA=Y apt install -y msodbcsql18

# Navigate to project
cd /home/user/training-matrix/cpp-skill-matrix

# Build the application
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
cmake --build . -j$(nproc)

# Run the application
./SkillMatrix
```

---

## Verifying Successful Installation

### Check 1: Qt is installed
```bash
qmake6 --version
# Expected: QMake version 3.1 using Qt version 6.x.x
```

### Check 2: CMake finds Qt
```bash
cd /home/user/training-matrix/cpp-skill-matrix/build
cmake ..
# Should complete without Qt-related errors
```

### Check 3: Application builds
```bash
cmake --build .
# Should compile without errors
```

### Check 4: Application runs
```bash
./SkillMatrix
# Login dialog should appear
```

---

## Next Steps After Installation

1. **Set up SQL Server database** (see BUILD_AND_TEST.md)
2. **Configure database connection** in `~/.skillmatrix/config.json`
3. **Run the application** and test login
4. **Check logs** in `~/.local/share/SkillMatrix/`

---

## Additional Resources

- **Qt Documentation**: https://doc.qt.io/qt-6/
- **Qt Downloads**: https://www.qt.io/download-qt-installer
- **CMake Qt Guide**: https://doc.qt.io/qt-6/cmake-manual.html
- **ODBC Driver**: https://docs.microsoft.com/en-us/sql/connect/odbc/linux-mac/installing-the-microsoft-odbc-driver-for-sql-server

---

**Last Updated**: 2025-12-11
**System**: Ubuntu 24.04 LTS
**Project**: C++ Skill Matrix Application
