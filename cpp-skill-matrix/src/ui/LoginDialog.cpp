#include "LoginDialog.h"
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
    setWindowTitle("Login - Skill Matrix");
    setModal(true);
    setFixedSize(500, 500);  // Taller to fit all content

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(50, 40, 50, 40);

    // Title
    QLabel* titleLabel = new QLabel("Skill Matrix", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    QLabel* subtitleLabel = new QLabel("Please login to continue", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);

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

    // Main layout - add everything directly
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(subtitleLabel);
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
    mainLayout->addSpacing(20);

    // Connections
    connect(loginButton_, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton_, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit_, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    // Load configuration and connect to database
    Config& config = Config::instance();
    config.load();

    // TODO: Show database connection dialog if not configured
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

    // Hash the password
    QString passwordHash = Crypto::hashPassword(password);

    // Authenticate via UserRepository
    UserRepository userRepo;
    User user = userRepo.authenticate(username, passwordHash);

    if (user.isValid()) {
        // Success
        Logger::instance().info("LoginDialog", "Login successful for user: " + username);

        // Set session with engineerId
        Application::instance().onUserLogin(user.id(), user.username(), user.role(), user.engineerId());

        statusLabel_->setText("Login successful!");
        statusLabel_->setStyleSheet("QLabel { color: green; }");
        loginButton_->setEnabled(true);
        return true;
    } else {
        // Failure
        Logger::instance().warning("LoginDialog", "Login failed for user: " + username);

        statusLabel_->setText("Invalid username or password");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        passwordEdit_->clear();
        passwordEdit_->setFocus();
        loginButton_->setEnabled(true);
        return false;
    }
}
