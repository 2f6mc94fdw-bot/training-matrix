#include "LoginDialog.h"
#include "DatabaseConnectionDialog.h"
#include "AptitudeLogoWidget.h"
#include "../core/Application.h"
#include "../core/Constants.h"
#include "../database/DatabaseManager.h"
#include "../database/UserRepository.h"
#include "../utils/Config.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"
#include "../utils/ValidationHelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , usernameEdit_(nullptr)
    , passwordEdit_(nullptr)
    , loginButton_(nullptr)
    , cancelButton_(nullptr)
    , statusLabel_(nullptr)
{
    setupUI();

    Logger::instance().info("LoginDialog", "Login dialog created");
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    setWindowTitle("Login - Aptitude");
    setModal(true);
    setFixedSize(500, 600);  // Taller to fit logo and content properly

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(50, 30, 50, 40);

    // Logo - use vector-based widget for crisp rendering
    AptitudeLogoWidget* logoWidget = new AptitudeLogoWidget(this);
    logoWidget->setSize(180);  // This sets size to 180x220 (180 + 40 for text)

    // Center the logo in a horizontal layout
    QHBoxLayout* logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(logoWidget);
    logoLayout->addStretch();

    // Username field
    QLabel* usernameLabel = new QLabel("Username", this);
    QFont labelFont = usernameLabel->font();
    labelFont.setPointSize(11);
    usernameLabel->setFont(labelFont);

    usernameEdit_ = new QLineEdit(this);
    usernameEdit_->setPlaceholderText("Enter username");
    usernameEdit_->setText("admin"); // Default for testing
    usernameEdit_->setMinimumHeight(40);
    usernameEdit_->setMaximumWidth(400);

    // Password field
    QLabel* passwordLabel = new QLabel("Password", this);
    passwordLabel->setFont(labelFont);

    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("Enter password");
    passwordEdit_->setText("admin123"); // Default for testing
    passwordEdit_->setMinimumHeight(40);
    passwordEdit_->setMaximumWidth(400);

    // Status label
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("QLabel { color: red; }");
    statusLabel_->setWordWrap(true);
    statusLabel_->setMinimumHeight(30);
    statusLabel_->setAlignment(Qt::AlignCenter);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    loginButton_ = new QPushButton("Login", this);
    loginButton_->setDefault(true);
    loginButton_->setMinimumHeight(45);
    loginButton_->setMinimumWidth(130);
    cancelButton_ = new QPushButton("Cancel", this);
    cancelButton_->setMinimumHeight(45);
    cancelButton_->setMinimumWidth(130);

    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton_);
    buttonLayout->addWidget(cancelButton_);
    buttonLayout->addStretch();

    // Main layout - add everything with proper spacing
    mainLayout->addLayout(logoLayout);
    mainLayout->addSpacing(40);
    mainLayout->addWidget(usernameLabel);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(usernameEdit_);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(passwordEdit_);
    mainLayout->addSpacing(25);
    mainLayout->addWidget(statusLabel_);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    // Connections
    connect(loginButton_, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton_, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit_, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    // Load configuration
    Config& config = Config::instance();
    config.load();

    // Attempt auto-connect using saved credentials
    if (!DatabaseManager::instance().isConnected()) {
        QString server = config.databaseServer();
        QString database = config.databaseName();
        QString user = config.databaseUser();
        QString password = config.databasePassword();
        int port = config.databasePort();

        // Try to auto-connect if we have saved credentials
        if (!server.isEmpty() && !database.isEmpty() && !user.isEmpty()) {
            Logger::instance().info("LoginDialog", "Attempting auto-connect to database");
            DatabaseManager::instance().connect(server, database, user, password, port);
        }

        // If still not connected, show the connection dialog
        if (!DatabaseManager::instance().isConnected()) {
            showDatabaseConnectionDialog();
        }
    }
}

void LoginDialog::showDatabaseConnectionDialog()
{
    DatabaseConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        // User canceled - show error message in login
        statusLabel_->setText("Database connection required to login");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        loginButton_->setEnabled(false);
    } else {
        // Connected successfully
        loginButton_->setEnabled(true);
    }
}

QString LoginDialog::username() const
{
    return usernameEdit_->text();
}

QString LoginDialog::password() const
{
    return passwordEdit_->text();
}

void LoginDialog::onLoginClicked()
{
    if (!validateInput()) {
        return;
    }

    if (attemptLogin()) {
        accept();
    }
}

void LoginDialog::onCancelClicked()
{
    reject();
}

bool LoginDialog::validateInput()
{
    statusLabel_->clear();

    QString username = usernameEdit_->text().trimmed();
    QString password = passwordEdit_->text();

    QString errorMessage;

    if (!ValidationHelper::validateRequired(username, "Username", errorMessage)) {
        statusLabel_->setText(errorMessage);
        usernameEdit_->setFocus();
        return false;
    }

    if (!ValidationHelper::validateRequired(password, "Password", errorMessage)) {
        statusLabel_->setText(errorMessage);
        passwordEdit_->setFocus();
        return false;
    }

    return true;
}

bool LoginDialog::attemptLogin()
{
    loginButton_->setEnabled(false);
    statusLabel_->setText("Logging in...");
    statusLabel_->setStyleSheet("QLabel { color: blue; }");

    QString username = usernameEdit_->text().trimmed();
    QString password = passwordEdit_->text();

    // Check database connection
    if (!DatabaseManager::instance().isConnected()) {
        statusLabel_->setText("Database not connected. Please check connection settings.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        loginButton_->setEnabled(true);
        Logger::instance().error("LoginDialog", "Database not connected");
        return false;
    }

    // Fetch user from database
    UserRepository userRepo;
    User user = userRepo.findByUsername(username);

    // Verify user exists and password matches using Crypto::verifyPassword
    if (!user.isValid() || !Crypto::verifyPassword(password, user.password())) {
        // Failure
        Logger::instance().warning("LoginDialog", "Login failed for user: " + username);

        statusLabel_->setText("Invalid username or password");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        passwordEdit_->clear();
        passwordEdit_->setFocus();
        loginButton_->setEnabled(true);
        return false;
    }

    // Success
    Logger::instance().info("LoginDialog", "Login successful for user: " + username);

    // Set session with engineerId
    Application::instance().onUserLogin(user.id(), user.username(), user.role(), user.engineerId());

    statusLabel_->setText("Login successful!");
    statusLabel_->setStyleSheet("QLabel { color: green; }");
    loginButton_->setEnabled(true);
    return true;
}
