cmake_minimum_required(VERSION 3.20)
project(Aptitude VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
    Charts
    PrintSupport
    Network
    Concurrent
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/controllers
    ${CMAKE_SOURCE_DIR}/src/database
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/utils
)

# Source files
set(SOURCES
    # Main
    src/main.cpp

    # Core
    src/core/Application.cpp
    src/core/Session.cpp

    # Models
    src/models/User.cpp
    src/models/Engineer.cpp
    src/models/ProductionArea.cpp
    src/models/Machine.cpp
    src/models/Competency.cpp
    src/models/Assessment.cpp
    src/models/CoreSkillCategory.cpp
    src/models/CoreSkill.cpp
    src/models/CoreSkillAssessment.cpp
    src/models/Certification.cpp
    src/models/Snapshot.cpp
    src/models/AuditLog.cpp

    # Database
    src/database/DatabaseManager.cpp
    src/database/UserRepository.cpp
    src/database/EngineerRepository.cpp
    src/database/ProductionRepository.cpp
    src/database/AssessmentRepository.cpp
    src/database/CoreSkillsRepository.cpp
    src/database/CertificationRepository.cpp
    src/database/SnapshotRepository.cpp
    src/database/AuditLogRepository.cpp

    # Controllers
    src/controllers/AuthController.cpp
    src/controllers/EngineerController.cpp
    src/controllers/ProductionController.cpp
    src/controllers/AssessmentController.cpp
    src/controllers/CoreSkillsController.cpp
    src/controllers/ReportController.cpp
    src/controllers/AnalyticsController.cpp
    src/controllers/CertificationController.cpp
    src/controllers/SnapshotController.cpp
    src/controllers/DataController.cpp

    # UI
    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/DatabaseConnectionDialog.cpp
    src/ui/DashboardWidget.cpp
    src/ui/EngineersWidget.cpp
    src/ui/UsersWidget.cpp
    src/ui/ProductionAreasWidget.cpp
    src/ui/AssessmentWidget.cpp
    src/ui/CoreSkillsWidget.cpp
    src/ui/CoreSkillsManagementWidget.cpp
    src/ui/MyCoreSkillsWidget.cpp
    src/ui/MyAssessmentsWidget.cpp
    src/ui/MyDashboardWidget.cpp
    src/ui/MyProgressWidget.cpp
    src/ui/ReportsWidget.cpp
    src/ui/AnalyticsWidget.cpp
    src/ui/CertificationsWidget.cpp
    src/ui/SnapshotsWidget.cpp
    src/ui/AuditLogWidget.cpp
    src/ui/SettingsDialog.cpp
    src/ui/ImportExportDialog.cpp
    src/ui/ChangePasswordDialog.cpp
    src/ui/StyleManager.cpp
    src/ui/AptitudeLogoWidget.cpp

    # UI Widgets
    src/ui/widgets/ChartWidget.cpp
    src/ui/widgets/ScoreEditor.cpp
    src/ui/widgets/TreeView.cpp
    src/ui/widgets/SearchBar.cpp

    # Utilities
    src/utils/Config.cpp
    src/utils/Logger.cpp
    src/utils/Crypto.cpp
    src/utils/ExcelImporter.cpp
    src/utils/ExcelExporter.cpp
    src/utils/JsonHelper.cpp
    src/utils/DateTimeHelper.cpp
    src/utils/ValidationHelper.cpp
    src/utils/IconProvider.cpp
)

set(HEADERS
    # Core
    src/core/Application.h
    src/core/Session.h
    src/core/Constants.h

    # Models
    src/models/User.h
    src/models/Engineer.h
    src/models/ProductionArea.h
    src/models/Machine.h
    src/models/Competency.h
    src/models/Assessment.h
    src/models/CoreSkillCategory.h
    src/models/CoreSkill.h
    src/models/CoreSkillAssessment.h
    src/models/Certification.h
    src/models/Snapshot.h
    src/models/AuditLog.h

    # Database
    src/database/DatabaseManager.h
    src/database/UserRepository.h
    src/database/EngineerRepository.h
    src/database/ProductionRepository.h
    src/database/AssessmentRepository.h
    src/database/CoreSkillsRepository.h
    src/database/CertificationRepository.h
    src/database/SnapshotRepository.h
    src/database/AuditLogRepository.h

    # Controllers
    src/controllers/AuthController.h
    src/controllers/EngineerController.h
    src/controllers/ProductionController.h
    src/controllers/AssessmentController.h
    src/controllers/CoreSkillsController.h
    src/controllers/ReportController.h
    src/controllers/AnalyticsController.h
    src/controllers/CertificationController.h
    src/controllers/SnapshotController.h
    src/controllers/DataController.h

    # UI
    src/ui/MainWindow.h
    src/ui/LoginDialog.h
    src/ui/DatabaseConnectionDialog.h
    src/ui/DashboardWidget.h
    src/ui/EngineersWidget.h
    src/ui/UsersWidget.h
    src/ui/ProductionAreasWidget.h
    src/ui/AssessmentWidget.h
    src/ui/CoreSkillsWidget.h
    src/ui/CoreSkillsManagementWidget.h
    src/ui/MyCoreSkillsWidget.h
    src/ui/MyAssessmentsWidget.h
    src/ui/MyDashboardWidget.h
    src/ui/MyProgressWidget.h
    src/ui/ReportsWidget.h
    src/ui/AnalyticsWidget.h
    src/ui/CertificationsWidget.h
    src/ui/SnapshotsWidget.h
    src/ui/AuditLogWidget.h
    src/ui/SettingsDialog.h
    src/ui/ImportExportDialog.h
    src/ui/ChangePasswordDialog.h
    src/ui/StyleManager.h
    src/ui/AptitudeLogoWidget.h

    # UI Widgets
    src/ui/widgets/ChartWidget.h
    src/ui/widgets/ScoreEditor.h
    src/ui/widgets/TreeView.h
    src/ui/widgets/SearchBar.h

    # Utilities
    src/utils/Config.h
    src/utils/Logger.h
    src/utils/Crypto.h
    src/utils/ExcelImporter.h
    src/utils/ExcelExporter.h
    src/utils/JsonHelper.h
    src/utils/DateTimeHelper.h
    src/utils/ValidationHelper.h
    src/utils/IconProvider.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::PrintSupport
    Qt6::Network
    Qt6::Concurrent
)

# Platform-specific settings
if(WIN32)
    # Windows: Create GUI application (no console)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    # macOS: Create app bundle with icon
    # TODO: Uncomment when aptitude.icns is created
    # set(MACOSX_BUNDLE_ICON_FILE aptitude.icns)
    # set(APP_ICON_MACOSX ${CMAKE_SOURCE_DIR}/resources/aptitude.icns)
    # set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
    #     MACOSX_PACKAGE_LOCATION "Resources"
    # )
    # target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_MACOSX})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        # MACOSX_BUNDLE_ICON_FILE aptitude.icns
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/Info.plist
    )
endif()

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# Copy resources
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
    DESTINATION share/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.qss" PATTERN "*.png" PATTERN "*.svg"
)

# Enable testing (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "========== Build Configuration ==========")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "=========================================")
message(STATUS "")
