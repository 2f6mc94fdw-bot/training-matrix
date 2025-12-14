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
    setFixedSize(450, 350);  // Comfortable size

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);  // Spacing between elements
    mainLayout->setContentsMargins(32, 32, 32, 32);  // Container margins

    // Title
    QLabel* titleLabel = new QLabel("Skill Matrix", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Login form
    QGroupBox* formGroup = new QGroupBox("Please login to continue", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    formLayout->setSpacing(16);  // Spacing between form rows
    formLayout->setContentsMargins(20, 20, 20, 20);  // Padding inside form group
    formLayout->setVerticalSpacing(16);  // Vertical spacing between rows
    formLayout->setHorizontalSpacing(12);  // Horizontal spacing between labels and fields

    usernameEdit_ = new QLineEdit(this);
    usernameEdit_->setPlaceholderText("Username");
    usernameEdit_->setText("admin"); // Default for testing

    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setText("admin123"); // Default for testing

    formLayout->addRow("Username:", usernameEdit_);
    formLayout->addRow("Password:", passwordEdit_);

    // Status label
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("QLabel { color: red; }");
    statusLabel_->setWordWrap(true);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);  // Spacing between buttons
    loginButton_ = new QPushButton("Login", this);
    loginButton_->setDefault(true);
    loginButton_->setMinimumHeight(36);  // Button height
    cancelButton_ = new QPushButton("Cancel", this);
    cancelButton_->setMinimumHeight(36);  // Button height

    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton_);
    buttonLayout->addWidget(cancelButton_);

    // Main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);  // Spacing after title
    mainLayout->addWidget(formGroup);
    mainLayout->addSpacing(12);  // Spacing before status label
    mainLayout->addWidget(statusLabel_);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

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

        // Set session
        Application::instance().onUserLogin(user.id(), user.username(), user.role());

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
