#include "UsersWidget.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"
#include "../core/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QInputDialog>

UsersWidget::UsersWidget(QWidget* parent)
    : QWidget(parent)
    , tableWidget_(nullptr)
    , addButton_(nullptr)
    , editButton_(nullptr)
    , deleteButton_(nullptr)
    , resetPasswordButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadUsers();
    Logger::instance().info("UsersWidget", "Users widget initialized");
}

UsersWidget::~UsersWidget()
{
}

void UsersWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    QLabel* titleLabel = new QLabel("User Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Table
    tableWidget_ = new QTableWidget(this);
    tableWidget_->setColumnCount(5);
    tableWidget_->setHorizontalHeaderLabels({"ID", "Username", "Role", "Engineer", "Created"});

    // Configure table appearance
    tableWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_->setAlternatingRowColors(true);
    tableWidget_->verticalHeader()->setVisible(false);
    tableWidget_->setShowGrid(true);

    // Configure column sizing
    QHeaderView* header = tableWidget_->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);  // ID
    header->setSectionResizeMode(1, QHeaderView::Stretch);           // Username
    header->setSectionResizeMode(2, QHeaderView::Fixed);             // Role
    header->setSectionResizeMode(3, QHeaderView::Stretch);           // Engineer
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);  // Created

    tableWidget_->setColumnWidth(2, 100);  // Role column width

    connect(tableWidget_, &QTableWidget::cellDoubleClicked, this, &UsersWidget::onTableDoubleClicked);

    mainLayout->addWidget(tableWidget_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    addButton_ = new QPushButton("Add User", this);
    editButton_ = new QPushButton("Edit User", this);
    deleteButton_ = new QPushButton("Delete User", this);
    resetPasswordButton_ = new QPushButton("Reset Password", this);
    refreshButton_ = new QPushButton("Refresh", this);

    // Set button sizes for consistency
    addButton_->setMinimumWidth(120);
    editButton_->setMinimumWidth(120);
    deleteButton_->setMinimumWidth(120);
    resetPasswordButton_->setMinimumWidth(140);
    refreshButton_->setMinimumWidth(100);

    connect(addButton_, &QPushButton::clicked, this, &UsersWidget::onAddClicked);
    connect(editButton_, &QPushButton::clicked, this, &UsersWidget::onEditClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &UsersWidget::onDeleteClicked);
    connect(resetPasswordButton_, &QPushButton::clicked, this, &UsersWidget::onResetPasswordClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &UsersWidget::onRefreshClicked);

    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addWidget(resetPasswordButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void UsersWidget::loadUsers()
{
    tableWidget_->setRowCount(0);

    QList<User> users = userRepository_.findAll();

    if (!userRepository_.lastError().isEmpty()) {
        Logger::instance().error("UsersWidget", "Failed to load users: " + userRepository_.lastError());
        QMessageBox::critical(this, "Error", "Failed to load users: " + userRepository_.lastError());
        return;
    }

    tableWidget_->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];

        tableWidget_->setItem(i, 0, new QTableWidgetItem(user.id()));
        tableWidget_->setItem(i, 1, new QTableWidgetItem(user.username()));
        tableWidget_->setItem(i, 2, new QTableWidgetItem(user.role()));

        // Engineer name or "N/A"
        QString engineerName = "N/A";
        if (!user.engineerId().isEmpty()) {
            engineerName = getEngineerName(user.engineerId());
        }
        tableWidget_->setItem(i, 3, new QTableWidgetItem(engineerName));

        tableWidget_->setItem(i, 4, new QTableWidgetItem(user.createdAt().toString("yyyy-MM-dd")));
    }

    Logger::instance().info("UsersWidget", QString("Loaded %1 users").arg(users.size()));
}

QString UsersWidget::getEngineerName(const QString& engineerId)
{
    if (engineerId.isEmpty()) {
        return "N/A";
    }

    Engineer engineer = engineerRepository_.findById(engineerId);
    if (engineer.isValid()) {
        return engineer.name();
    }

    return "Unknown";
}

void UsersWidget::showUserDialog(const User* user)
{
    QDialog dialog(this);
    dialog.setWindowTitle(user ? "Edit User" : "Add User");
    dialog.resize(450, 300);

    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* idEdit = new QLineEdit(&dialog);
    QLineEdit* usernameEdit = new QLineEdit(&dialog);
    QComboBox* roleCombo = new QComboBox(&dialog);
    QComboBox* engineerCombo = new QComboBox(&dialog);
    QLineEdit* passwordEdit = new QLineEdit(&dialog);

    // Role options
    roleCombo->addItems({Constants::ROLE_ADMIN, Constants::ROLE_ENGINEER});

    // Engineer options
    engineerCombo->addItem("None", "");
    QList<Engineer> engineers = engineerRepository_.findAll();
    for (const Engineer& eng : engineers) {
        engineerCombo->addItem(eng.name(), eng.id());
    }

    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(user ? "Leave blank to keep current" : "Enter password");

    if (user) {
        idEdit->setText(user->id());
        idEdit->setReadOnly(true);
        usernameEdit->setText(user->username());
        roleCombo->setCurrentText(user->role());

        // Set engineer selection
        if (!user->engineerId().isEmpty()) {
            int index = engineerCombo->findData(user->engineerId());
            if (index >= 0) {
                engineerCombo->setCurrentIndex(index);
            }
        }
    } else {
        idEdit->setPlaceholderText("Auto-generated");
        idEdit->setReadOnly(true);
    }

    formLayout->addRow("ID:", idEdit);
    formLayout->addRow("Username:", usernameEdit);
    formLayout->addRow("Password:", passwordEdit);
    formLayout->addRow("Role:", roleCombo);
    formLayout->addRow("Engineer:", engineerCombo);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text();
        QString role = roleCombo->currentText();
        QString engineerId = engineerCombo->currentData().toString();

        if (username.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Username is required.");
            return;
        }

        if (!user && password.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Password is required for new users.");
            return;
        }

        bool success = false;
        if (user) {
            // Update existing user
            User updatedUser = *user;
            updatedUser.setUsername(username);
            updatedUser.setRole(role);
            updatedUser.setEngineerId(engineerId);

            success = userRepository_.update(updatedUser);

            // Update password if provided
            if (!password.isEmpty() && success) {
                QString passwordHash = Crypto::hashPassword(password);
                success = userRepository_.updatePassword(updatedUser.id(), passwordHash);
            }

            if (success) {
                Logger::instance().info("UsersWidget", "Updated user: " + username);
                QMessageBox::information(this, "Success", "User updated successfully.");
            }
        } else {
            // Create new user
            QString userId = Crypto::generateId("usr");
            QString passwordHash = Crypto::hashPassword(password);

            User newUser(userId, username, passwordHash, role, engineerId);
            success = userRepository_.save(newUser);

            if (success) {
                Logger::instance().info("UsersWidget", "Created user: " + username);
                QMessageBox::information(this, "Success",
                    QString("User created successfully.\nUsername: %1\nPassword: %2\n\nPlease save this password securely.")
                    .arg(username).arg(password));
            }
        }

        if (!success) {
            Logger::instance().error("UsersWidget", "Failed to save user: " + userRepository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save user: " + userRepository_.lastError());
        } else {
            loadUsers();
        }
    }
}

void UsersWidget::showPasswordResetDialog(const User& user)
{
    bool ok;
    QString newPassword = QInputDialog::getText(this, "Reset Password",
        QString("Enter new password for user '%1':").arg(user.username()),
        QLineEdit::Password, "", &ok);

    if (ok && !newPassword.isEmpty()) {
        QString passwordHash = Crypto::hashPassword(newPassword);
        bool success = userRepository_.updatePassword(user.id(), passwordHash);

        if (success) {
            Logger::instance().info("UsersWidget", "Password reset for user: " + user.username());
            QMessageBox::information(this, "Success",
                QString("Password reset successfully.\n\nUsername: %1\nNew Password: %2\n\nPlease provide this to the user securely.")
                .arg(user.username()).arg(newPassword));
        } else {
            Logger::instance().error("UsersWidget", "Failed to reset password: " + userRepository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to reset password: " + userRepository_.lastError());
        }
    } else if (ok) {
        QMessageBox::warning(this, "Validation Error", "Password cannot be empty.");
    }
}

void UsersWidget::onAddClicked()
{
    showUserDialog(nullptr);
}

void UsersWidget::onEditClicked()
{
    int currentRow = tableWidget_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a user to edit.");
        return;
    }

    QString id = tableWidget_->item(currentRow, 0)->text();
    User user = userRepository_.findById(id);

    if (user.id().isEmpty()) {
        QMessageBox::critical(this, "Error", "Failed to load user data.");
        return;
    }

    showUserDialog(&user);
}

void UsersWidget::onDeleteClicked()
{
    int currentRow = tableWidget_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a user to delete.");
        return;
    }

    QString id = tableWidget_->item(currentRow, 0)->text();
    QString username = tableWidget_->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete user '%1'?\n\nThis action cannot be undone.").arg(username),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        bool success = userRepository_.remove(id);

        if (success) {
            Logger::instance().info("UsersWidget", "Deleted user: " + id);
            QMessageBox::information(this, "Success", "User deleted successfully.");
            loadUsers();
        } else {
            Logger::instance().error("UsersWidget", "Failed to delete user: " + userRepository_.lastError());
            QMessageBox::critical(this, "Error", "Failed to delete user: " + userRepository_.lastError());
        }
    }
}

void UsersWidget::onResetPasswordClicked()
{
    int currentRow = tableWidget_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a user to reset password.");
        return;
    }

    QString id = tableWidget_->item(currentRow, 0)->text();
    User user = userRepository_.findById(id);

    if (user.id().isEmpty()) {
        QMessageBox::critical(this, "Error", "Failed to load user data.");
        return;
    }

    showPasswordResetDialog(user);
}

void UsersWidget::onRefreshClicked()
{
    loadUsers();
}

void UsersWidget::onTableDoubleClicked(int row, int column)
{
    QString id = tableWidget_->item(row, 0)->text();
    User user = userRepository_.findById(id);

    if (user.id().isEmpty()) {
        QMessageBox::critical(this, "Error", "Failed to load user data.");
        return;
    }

    showUserDialog(&user);
}
