#include "ChangePasswordDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>

ChangePasswordDialog::ChangePasswordDialog(QWidget* parent)
    : QDialog(parent)
    , oldPasswordEdit_(nullptr)
    , newPasswordEdit_(nullptr)
    , confirmPasswordEdit_(nullptr)
    , statusLabel_(nullptr)
    , strengthLabel_(nullptr)
{
    setupUI();
}

ChangePasswordDialog::~ChangePasswordDialog()
{
}

void ChangePasswordDialog::setupUI()
{
    setWindowTitle("Change Password");
    setModal(true);
    setFixedSize(450, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    QLabel* titleLabel = new QLabel("Change Your Password", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    oldPasswordEdit_ = new QLineEdit(this);
    oldPasswordEdit_->setEchoMode(QLineEdit::Password);
    oldPasswordEdit_->setMinimumHeight(35);
    oldPasswordEdit_->setPlaceholderText("Enter current password");

    newPasswordEdit_ = new QLineEdit(this);
    newPasswordEdit_->setEchoMode(QLineEdit::Password);
    newPasswordEdit_->setMinimumHeight(35);
    newPasswordEdit_->setPlaceholderText("Enter new password");

    confirmPasswordEdit_ = new QLineEdit(this);
    confirmPasswordEdit_->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit_->setMinimumHeight(35);
    confirmPasswordEdit_->setPlaceholderText("Confirm new password");

    formLayout->addRow("Current Password:", oldPasswordEdit_);
    formLayout->addRow("New Password:", newPasswordEdit_);
    formLayout->addRow("Confirm Password:", confirmPasswordEdit_);

    mainLayout->addLayout(formLayout);

    // Password strength indicator
    strengthLabel_ = new QLabel(this);
    strengthLabel_->setStyleSheet("QLabel { color: gray; font-style: italic; }");
    strengthLabel_->setWordWrap(true);
    mainLayout->addWidget(strengthLabel_);

    // Status label
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("QLabel { color: red; }");
    statusLabel_->setWordWrap(true);
    statusLabel_->setMinimumHeight(20);
    mainLayout->addWidget(statusLabel_);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ChangePasswordDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(newPasswordEdit_, &QLineEdit::textChanged, this, &ChangePasswordDialog::onPasswordChanged);
    connect(confirmPasswordEdit_, &QLineEdit::textChanged, this, &ChangePasswordDialog::onPasswordChanged);

    setLayout(mainLayout);
}

void ChangePasswordDialog::onPasswordChanged()
{
    QString newPwd = newPasswordEdit_->text();

    // Clear status when user types
    statusLabel_->clear();

    // Show password strength
    if (newPwd.isEmpty()) {
        strengthLabel_->clear();
        return;
    }

    int strength = 0;
    if (newPwd.length() >= 8) strength++;
    if (newPwd.length() >= 12) strength++;
    if (newPwd.contains(QRegularExpression("[A-Z]"))) strength++;
    if (newPwd.contains(QRegularExpression("[a-z]"))) strength++;
    if (newPwd.contains(QRegularExpression("[0-9]"))) strength++;
    if (newPwd.contains(QRegularExpression("[^A-Za-z0-9]"))) strength++;

    QString strengthText;
    QString strengthColor;
    if (strength < 3) {
        strengthText = "Weak password";
        strengthColor = "red";
    } else if (strength < 5) {
        strengthText = "Moderate password";
        strengthColor = "orange";
    } else {
        strengthText = "Strong password";
        strengthColor = "green";
    }

    strengthLabel_->setText(QString("Password strength: %1").arg(strengthText));
    strengthLabel_->setStyleSheet(QString("QLabel { color: %1; font-style: italic; }").arg(strengthColor));
}

bool ChangePasswordDialog::validatePasswords()
{
    if (oldPasswordEdit_->text().isEmpty()) {
        statusLabel_->setText("Current password is required.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        oldPasswordEdit_->setFocus();
        return false;
    }

    if (newPasswordEdit_->text().isEmpty()) {
        statusLabel_->setText("New password is required.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        newPasswordEdit_->setFocus();
        return false;
    }

    if (newPasswordEdit_->text().length() < 6) {
        statusLabel_->setText("New password must be at least 6 characters long.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        newPasswordEdit_->setFocus();
        return false;
    }

    if (newPasswordEdit_->text() != confirmPasswordEdit_->text()) {
        statusLabel_->setText("New passwords do not match.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        confirmPasswordEdit_->setFocus();
        return false;
    }

    if (oldPasswordEdit_->text() == newPasswordEdit_->text()) {
        statusLabel_->setText("New password must be different from current password.");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        newPasswordEdit_->setFocus();
        return false;
    }

    return true;
}

void ChangePasswordDialog::onAccepted()
{
    if (validatePasswords()) {
        accept();
    }
}
