#include "DatabaseConnectionDialog.h"
#include "../core/Constants.h"
#include "../database/DatabaseManager.h"
#include "../utils/Config.h"
#include "../utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
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
    setFixedSize(640, 620);
    setStyleSheet(
        "QDialog { background-color: #f8fafc; color: #0f172a; }"
        "QGroupBox { color: #0f172a; font-weight: 600; }"
        "QLabel { color: #0f172a; }"
        "QLineEdit, QSpinBox {"
        "  background-color: #ffffff;"
        "  color: #0f172a;"
        "  border: 1px solid #cbd5e1;"
        "  border-radius: 6px;"
        "  padding: 6px 8px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus {"
        "  border: 2px solid #3b82f6;"
        "}"
        "QPushButton {"
        "  min-height: 36px;"
        "  border-radius: 6px;"
        "  padding: 6px 12px;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(28, 22, 28, 22);

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
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    QLabel* subtitleLabel = new QLabel("Configure your database connection", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(11);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #666;");

    // Connection form
    QGroupBox* formGroup = new QGroupBox("Connection Details", this);
    QGridLayout* formLayout = new QGridLayout(formGroup);
    formLayout->setHorizontalSpacing(12);
    formLayout->setVerticalSpacing(10);
    formLayout->setContentsMargins(20, 20, 20, 20);

    auto makeFieldLabel = [this](const QString& text) -> QLabel* {
        QLabel* label = new QLabel(text, this);
        QFont labelFont = label->font();
        labelFont.setPointSize(11);
        labelFont.setBold(true);
        label->setFont(labelFont);
        label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        return label;
    };

    QLabel* serverLabel = makeFieldLabel("Server:");
    QLabel* portLabel = makeFieldLabel("Port:");
    QLabel* databaseLabel = makeFieldLabel("Database:");
    QLabel* usernameLabel2 = makeFieldLabel("Username:");
    QLabel* passwordLabel2 = makeFieldLabel("Password:");

    // Server
    serverEdit_ = new QLineEdit(this);
    serverEdit_->setPlaceholderText("localhost or IP address");
    serverEdit_->setMinimumHeight(35);
    serverEdit_->setMinimumWidth(320);

    // Port
    portSpinBox_ = new QSpinBox(this);
    portSpinBox_->setRange(1, 65535);
    portSpinBox_->setValue(1433);
    portSpinBox_->setMinimumHeight(35);
    portSpinBox_->setMinimumWidth(320);

    // Database
    databaseEdit_ = new QLineEdit(this);
    databaseEdit_->setPlaceholderText("Database name");
    databaseEdit_->setMinimumHeight(35);
    databaseEdit_->setMinimumWidth(320);

    // Username
    usernameEdit_ = new QLineEdit(this);
    usernameEdit_->setPlaceholderText("SQL Server username");
    usernameEdit_->setMinimumHeight(35);
    usernameEdit_->setMinimumWidth(320);

    // Password
    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("SQL Server password");
    passwordEdit_->setMinimumHeight(35);
    passwordEdit_->setMinimumWidth(320);

    formLayout->addWidget(serverLabel, 0, 0);
    formLayout->addWidget(serverEdit_, 0, 1);
    formLayout->addWidget(portLabel, 1, 0);
    formLayout->addWidget(portSpinBox_, 1, 1);
    formLayout->addWidget(databaseLabel, 2, 0);
    formLayout->addWidget(databaseEdit_, 2, 1);
    formLayout->addWidget(usernameLabel2, 3, 0);
    formLayout->addWidget(usernameEdit_, 3, 1);
    formLayout->addWidget(passwordLabel2, 4, 0);
    formLayout->addWidget(passwordEdit_, 4, 1);

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
    mainLayout->addSpacing(2);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(formGroup);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(statusLabel_);
    mainLayout->addSpacing(8);
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
    config.load(); // Ensure config is loaded before reading values

    // Load last used connection settings using Config's database helpers
    QString lastServer = config.databaseServer();
    QString lastDatabase = config.databaseName();
    QString lastUser = config.databaseUser();
    int lastPort = config.databasePort();

    serverEdit_->setText(lastServer);
    databaseEdit_->setText(lastDatabase);
    usernameEdit_->setText(lastUser);
    passwordEdit_->clear();
    portSpinBox_->setValue(lastPort);

    // Focus on first empty field, or password if all filled
    if (!lastServer.isEmpty() && !lastDatabase.isEmpty() && !lastUser.isEmpty()) {
        passwordEdit_->setFocus();
    } else {
        serverEdit_->setFocus();
    }
}

void DatabaseConnectionDialog::saveSettings()
{
    Config& config = Config::instance();

    // Save using Config's database helper methods
    config.setDatabaseServer(serverEdit_->text());
    config.setDatabaseName(databaseEdit_->text());
    config.setDatabaseUser(usernameEdit_->text());
    config.setDatabasePort(portSpinBox_->value());

    // Do not persist database password in plaintext config.
    config.remove("database.password");
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
        port(),
        Config::instance().databaseEncrypt(),
        Config::instance().databaseTrustServerCertificate()
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
        port(),
        Config::instance().databaseEncrypt(),
        Config::instance().databaseTrustServerCertificate()
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
