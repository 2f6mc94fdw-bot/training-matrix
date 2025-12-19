#include "DatabaseConnectionDialog.h"
#include "../core/Constants.h"
#include "../database/DatabaseManager.h"
#include "../utils/Config.h"
#include "../utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QPixmap>

DatabaseConnectionDialog::DatabaseConnectionDialog(QWidget* parent)
    : QDialog(parent)
    , serverEdit_(nullptr)
    , databaseEdit_(nullptr)
    , usernameEdit_(nullptr)
    , passwordEdit_(nullptr)
    , portSpinBox_(nullptr)
    , testButton_(nullptr)
    , connectButton_(nullptr)
    , cancelButton_(nullptr)
    , statusLabel_(nullptr)
{
    setupUI();
    loadSavedSettings();

    Logger::instance().info("DatabaseConnectionDialog", "Database connection dialog created");
}

DatabaseConnectionDialog::~DatabaseConnectionDialog()
{
}

void DatabaseConnectionDialog::setupUI()
{
    setWindowTitle("Database Connection - Aptitude");
    setModal(true);
    setFixedSize(550, 450);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    // Logo
    QLabel* logoLabel = new QLabel(this);
    QPixmap logo(":/images/aptitude-logo.png");
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignCenter);
    } else {
        logoLabel->setText("APTITUDE");
        QFont logoFont = logoLabel->font();
        logoFont.setPointSize(18);
        logoFont.setBold(true);
        logoLabel->setFont(logoFont);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setStyleSheet(QString("color: %1;").arg(Constants::BRAND_LIGHT_BLUE));
    }

    // Title
    QLabel* titleLabel = new QLabel("SQL Server Connection", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    QLabel* subtitleLabel = new QLabel("Configure your database connection", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(10);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #666;");

    // Connection form
    QGroupBox* formGroup = new QGroupBox("Connection Details", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(20, 20, 20, 20);

    // Server
    serverEdit_ = new QLineEdit(this);
    serverEdit_->setPlaceholderText("localhost or IP address");
    serverEdit_->setMinimumHeight(35);
    formLayout->addRow("Server:", serverEdit_);

    // Port
    portSpinBox_ = new QSpinBox(this);
    portSpinBox_->setRange(1, 65535);
    portSpinBox_->setValue(1433);
    portSpinBox_->setMinimumHeight(35);
    formLayout->addRow("Port:", portSpinBox_);

    // Database
    databaseEdit_ = new QLineEdit(this);
    databaseEdit_->setPlaceholderText("Database name");
    databaseEdit_->setMinimumHeight(35);
    formLayout->addRow("Database:", databaseEdit_);

    // Username
    usernameEdit_ = new QLineEdit(this);
    usernameEdit_->setPlaceholderText("SQL Server username");
    usernameEdit_->setMinimumHeight(35);
    formLayout->addRow("Username:", usernameEdit_);

    // Password
    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("SQL Server password");
    passwordEdit_->setMinimumHeight(35);
    formLayout->addRow("Password:", passwordEdit_);

    // Status label
    statusLabel_ = new QLabel(this);
    statusLabel_->setWordWrap(true);
    statusLabel_->setMinimumHeight(40);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet("QLabel { padding: 8px; border-radius: 4px; }");

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    testButton_ = new QPushButton("Test Connection", this);
    testButton_->setMinimumHeight(40);
    testButton_->setMinimumWidth(120);

    connectButton_ = new QPushButton("Connect", this);
    connectButton_->setDefault(true);
    connectButton_->setMinimumHeight(40);
    connectButton_->setMinimumWidth(120);

    cancelButton_ = new QPushButton("Cancel", this);
    cancelButton_->setMinimumHeight(40);
    cancelButton_->setMinimumWidth(120);

    buttonLayout->addWidget(testButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(connectButton_);
    buttonLayout->addWidget(cancelButton_);

    // Main layout
    mainLayout->addWidget(logoLabel);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(formGroup);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(statusLabel_);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(testButton_, &QPushButton::clicked, this, &DatabaseConnectionDialog::onTestConnection);
    connect(connectButton_, &QPushButton::clicked, this, &DatabaseConnectionDialog::onConnect);
    connect(cancelButton_, &QPushButton::clicked, this, &DatabaseConnectionDialog::onCancel);
    connect(serverEdit_, &QLineEdit::returnPressed, this, &DatabaseConnectionDialog::onConnect);
    connect(databaseEdit_, &QLineEdit::returnPressed, this, &DatabaseConnectionDialog::onConnect);
    connect(usernameEdit_, &QLineEdit::returnPressed, this, &DatabaseConnectionDialog::onConnect);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &DatabaseConnectionDialog::onConnect);
}

void DatabaseConnectionDialog::loadSavedSettings()
{
    Config& config = Config::instance();

    // Load last used connection settings
    QString lastServer = config.value(Constants::SETTING_LAST_DB_SERVER, "localhost").toString();
    QString lastDatabase = config.value(Constants::SETTING_LAST_DB_NAME, "SkillMatrix").toString();
    QString lastUser = config.value(Constants::SETTING_LAST_DB_USER, "sa").toString();

    serverEdit_->setText(lastServer);
    databaseEdit_->setText(lastDatabase);
    usernameEdit_->setText(lastUser);

    // Focus on password field if other fields are filled
    if (!lastServer.isEmpty() && !lastDatabase.isEmpty() && !lastUser.isEmpty()) {
        passwordEdit_->setFocus();
    } else {
        serverEdit_->setFocus();
    }
}

void DatabaseConnectionDialog::saveSettings()
{
    Config& config = Config::instance();

    config.setValue(Constants::SETTING_LAST_DB_SERVER, serverEdit_->text());
    config.setValue(Constants::SETTING_LAST_DB_NAME, databaseEdit_->text());
    config.setValue(Constants::SETTING_LAST_DB_USER, usernameEdit_->text());

    config.save();
}

QString DatabaseConnectionDialog::server() const
{
    return serverEdit_->text().trimmed();
}

QString DatabaseConnectionDialog::database() const
{
    return databaseEdit_->text().trimmed();
}

QString DatabaseConnectionDialog::username() const
{
    return usernameEdit_->text().trimmed();
}

QString DatabaseConnectionDialog::password() const
{
    return passwordEdit_->text();
}

int DatabaseConnectionDialog::port() const
{
    return portSpinBox_->value();
}

void DatabaseConnectionDialog::onTestConnection()
{
    statusLabel_->clear();

    // Validate inputs
    if (server().isEmpty() || database().isEmpty() || username().isEmpty()) {
        statusLabel_->setText("⚠ Please fill in all required fields");
        statusLabel_->setStyleSheet("QLabel { color: #FB923C; background-color: #FFF7ED; padding: 8px; border-radius: 4px; }");
        return;
    }

    testButton_->setEnabled(false);
    connectButton_->setEnabled(false);
    statusLabel_->setText("⏳ Testing connection...");
    statusLabel_->setStyleSheet("QLabel { color: #60A5FA; background-color: #EFF6FF; padding: 8px; border-radius: 4px; }");

    // Test connection
    bool success = DatabaseManager::instance().connect(
        server(),
        database(),
        username(),
        password(),
        port()
    );

    if (success) {
        statusLabel_->setText("✓ Connection successful!");
        statusLabel_->setStyleSheet("QLabel { color: #10B981; background-color: #ECFDF5; padding: 8px; border-radius: 4px; }");
        Logger::instance().info("DatabaseConnectionDialog", "Connection test successful");
    } else {
        QString error = DatabaseManager::instance().lastError();
        statusLabel_->setText("✗ Connection failed: " + error);
        statusLabel_->setStyleSheet("QLabel { color: #EF4444; background-color: #FEF2F2; padding: 8px; border-radius: 4px; }");
        Logger::instance().error("DatabaseConnectionDialog", "Connection test failed: " + error);

        // Disconnect since test failed
        DatabaseManager::instance().disconnect();
    }

    testButton_->setEnabled(true);
    connectButton_->setEnabled(true);
}

void DatabaseConnectionDialog::onConnect()
{
    statusLabel_->clear();

    // Validate inputs
    if (server().isEmpty() || database().isEmpty() || username().isEmpty()) {
        statusLabel_->setText("⚠ Please fill in all required fields");
        statusLabel_->setStyleSheet("QLabel { color: #FB923C; background-color: #FFF7ED; padding: 8px; border-radius: 4px; }");
        return;
    }

    testButton_->setEnabled(false);
    connectButton_->setEnabled(false);
    statusLabel_->setText("⏳ Connecting to database...");
    statusLabel_->setStyleSheet("QLabel { color: #60A5FA; background-color: #EFF6FF; padding: 8px; border-radius: 4px; }");

    // Connect to database
    bool success = DatabaseManager::instance().connect(
        server(),
        database(),
        username(),
        password(),
        port()
    );

    if (success) {
        statusLabel_->setText("✓ Connected successfully!");
        statusLabel_->setStyleSheet("QLabel { color: #10B981; background-color: #ECFDF5; padding: 8px; border-radius: 4px; }");
        Logger::instance().info("DatabaseConnectionDialog", "Database connection successful");

        // Save settings and accept dialog
        saveSettings();
        accept();
    } else {
        QString error = DatabaseManager::instance().lastError();
        statusLabel_->setText("✗ Connection failed: " + error);
        statusLabel_->setStyleSheet("QLabel { color: #EF4444; background-color: #FEF2F2; padding: 8px; border-radius: 4px; }");
        Logger::instance().error("DatabaseConnectionDialog", "Database connection failed: " + error);

        testButton_->setEnabled(true);
        connectButton_->setEnabled(true);
    }
}

void DatabaseConnectionDialog::onCancel()
{
    Logger::instance().info("DatabaseConnectionDialog", "Connection canceled");
    reject();
}
