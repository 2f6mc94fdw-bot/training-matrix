#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {

// Application Info
constexpr const char* APP_NAME = "Aptitude";
constexpr const char* APP_VERSION = "1.0.0";
constexpr const char* APP_ORGANIZATION = "Training Matrix";
constexpr const char* APP_DOMAIN = "aptitude.local";

// Database
constexpr const char* DB_CONNECTION_NAME = "SkillMatrixDB";
constexpr int DB_CONNECTION_TIMEOUT = 5000; // milliseconds

// User Roles
constexpr const char* ROLE_ADMIN = "admin";
constexpr const char* ROLE_ENGINEER = "engineer";

// Shifts
constexpr const char* SHIFT_A = "A Shift";
constexpr const char* SHIFT_B = "B Shift";
constexpr const char* SHIFT_C = "C Shift";
constexpr const char* SHIFT_D = "D Shift";
constexpr const char* SHIFT_DAY = "Day Shift";

// Score Levels
constexpr int SCORE_NOT_TRAINED = 0;
constexpr int SCORE_BASIC = 1;
constexpr int SCORE_COMPETENT = 2;
constexpr int SCORE_EXPERT = 3;
constexpr int SCORE_MAX = 3;

// Score Labels
constexpr const char* SCORE_LABEL_0 = "Not Trained";
constexpr const char* SCORE_LABEL_1 = "Basic/Awareness";
constexpr const char* SCORE_LABEL_2 = "Competent/Independent";
constexpr const char* SCORE_LABEL_3 = "Expert/Can Train";

// Score Colors (for UI)
constexpr const char* COLOR_SCORE_0 = "#dc2626"; // Red
constexpr const char* COLOR_SCORE_1 = "#f59e0b"; // Yellow/Orange
constexpr const char* COLOR_SCORE_2 = "#3b82f6"; // Blue
constexpr const char* COLOR_SCORE_3 = "#10b981"; // Green

// Priority Levels
constexpr const char* PRIORITY_LOW = "Low";
constexpr const char* PRIORITY_MEDIUM = "Medium";
constexpr const char* PRIORITY_HIGH = "High";
constexpr const char* PRIORITY_CRITICAL = "Critical";

// Core Skill Categories (Default)
constexpr const char* CATEGORY_MECHANICAL = "Mechanical";
constexpr const char* CATEGORY_ELECTRICAL = "Electrical";
constexpr const char* CATEGORY_SOFTWARE = "Software";
constexpr const char* CATEGORY_SAFETY = "Safety";
constexpr const char* CATEGORY_LEADERSHIP = "Leadership";

// File Filters
constexpr const char* FILTER_EXCEL = "Excel Files (*.xlsx *.xls)";
constexpr const char* FILTER_JSON = "JSON Files (*.json)";
constexpr const char* FILTER_CSV = "CSV Files (*.csv)";
constexpr const char* FILTER_PDF = "PDF Files (*.pdf)";
constexpr const char* FILTER_ALL = "All Files (*)";

// UI Constants
constexpr int SIDEBAR_WIDTH = 200;
constexpr int SIDEBAR_WIDTH_COLLAPSED = 60;
constexpr int MIN_WINDOW_WIDTH = 1024;
constexpr int MIN_WINDOW_HEIGHT = 768;
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 900;

// Table/Grid
constexpr int TABLE_ROW_HEIGHT = 35;
constexpr int TABLE_HEADER_HEIGHT = 40;

// Session
constexpr int SESSION_TIMEOUT = 3600000; // 1 hour in milliseconds
constexpr int AUTO_SAVE_INTERVAL = 300000; // 5 minutes in milliseconds

// Password
constexpr int PASSWORD_MIN_LENGTH = 6;
constexpr int PASSWORD_MAX_LENGTH = 255;
constexpr int BCRYPT_ROUNDS = 10;

// Pagination
constexpr int DEFAULT_PAGE_SIZE = 50;
constexpr int MAX_PAGE_SIZE = 1000;

// Export
constexpr int EXPORT_BATCH_SIZE = 100;
constexpr const char* EXPORT_DATE_FORMAT = "yyyy-MM-dd";
constexpr const char* EXPORT_DATETIME_FORMAT = "yyyy-MM-dd HH:mm:ss";

// Audit Actions
constexpr const char* ACTION_LOGIN = "LOGIN";
constexpr const char* ACTION_LOGOUT = "LOGOUT";
constexpr const char* ACTION_CREATE = "CREATE";
constexpr const char* ACTION_UPDATE = "UPDATE";
constexpr const char* ACTION_DELETE = "DELETE";
constexpr const char* ACTION_IMPORT = "IMPORT";
constexpr const char* ACTION_EXPORT = "EXPORT";
constexpr const char* ACTION_BACKUP = "BACKUP";
constexpr const char* ACTION_RESTORE = "RESTORE";

// Settings Keys
constexpr const char* SETTING_THEME = "ui/theme";
constexpr const char* SETTING_WINDOW_GEOMETRY = "ui/windowGeometry";
constexpr const char* SETTING_WINDOW_STATE = "ui/windowState";
constexpr const char* SETTING_SIDEBAR_VISIBLE = "ui/sidebarVisible";
constexpr const char* SETTING_LAST_DB_SERVER = "database/lastServer";
constexpr const char* SETTING_LAST_DB_NAME = "database/lastDatabase";
constexpr const char* SETTING_LAST_DB_USER = "database/lastUser";
constexpr const char* SETTING_AUTO_SAVE = "app/autoSave";
constexpr const char* SETTING_SHOW_TIPS = "app/showTips";

// Themes
constexpr const char* THEME_LIGHT = "light";
constexpr const char* THEME_DARK = "dark";

// Chart Colors
constexpr const char* CHART_COLOR_1 = "#3b82f6"; // Blue
constexpr const char* CHART_COLOR_2 = "#10b981"; // Green
constexpr const char* CHART_COLOR_3 = "#f59e0b"; // Orange
constexpr const char* CHART_COLOR_4 = "#8b5cf6"; // Purple
constexpr const char* CHART_COLOR_5 = "#ef4444"; // Red
constexpr const char* CHART_COLOR_6 = "#06b6d4"; // Cyan

// Aptitude Brand Colors (from logo)
constexpr const char* BRAND_PRIMARY_BLUE = "#1E3A8A";   // Dark blue background
constexpr const char* BRAND_LIGHT_BLUE = "#60A5FA";     // Light blue
constexpr const char* BRAND_CYAN = "#22D3EE";            // Cyan/Teal
constexpr const char* BRAND_GREEN = "#10B981";           // Green
constexpr const char* BRAND_YELLOW = "#FDE047";          // Yellow
constexpr const char* BRAND_ORANGE = "#FB923C";          // Orange
constexpr const char* BRAND_RED = "#EF4444";             // Red
constexpr const char* BRAND_ACCENT = "#60A5FA";          // Accent color for highlights

// Error Messages
constexpr const char* ERROR_DB_CONNECTION = "Failed to connect to database";
constexpr const char* ERROR_DB_QUERY = "Database query failed";
constexpr const char* ERROR_INVALID_CREDENTIALS = "Invalid username or password";
constexpr const char* ERROR_PERMISSION_DENIED = "Permission denied";
constexpr const char* ERROR_INVALID_INPUT = "Invalid input data";
constexpr const char* ERROR_RECORD_NOT_FOUND = "Record not found";
constexpr const char* ERROR_DUPLICATE_ENTRY = "Duplicate entry";
constexpr const char* ERROR_FILE_NOT_FOUND = "File not found";
constexpr const char* ERROR_FILE_READ = "Failed to read file";
constexpr const char* ERROR_FILE_WRITE = "Failed to write file";

// Success Messages
constexpr const char* SUCCESS_SAVE = "Data saved successfully";
constexpr const char* SUCCESS_DELETE = "Record deleted successfully";
constexpr const char* SUCCESS_IMPORT = "Data imported successfully";
constexpr const char* SUCCESS_EXPORT = "Data exported successfully";
constexpr const char* SUCCESS_BACKUP = "Backup created successfully";
constexpr const char* SUCCESS_RESTORE = "Data restored successfully";

} // namespace Constants

#endif // CONSTANTS_H
