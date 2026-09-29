#include "MyCertificationsWidget.h"
#include "../core/Application.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QShowEvent>
#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QUrl>
#include <QMessageBox>

MyCertificationsWidget::MyCertificationsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , isFirstShow_(true)
    , summaryLabel_(nullptr)
    , certificationsTable_(nullptr)
    , uploadButton_(nullptr)
    , openButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
}

MyCertificationsWidget::~MyCertificationsWidget()
{
}

void MyCertificationsWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (isFirstShow_) {
        isFirstShow_ = false;
        loadCertifications();
    }
}

void MyCertificationsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("My Certifications", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    uploadButton_ = new QPushButton("Upload Certificate", this);
    connect(uploadButton_, &QPushButton::clicked, this, &MyCertificationsWidget::onUploadClicked);
    headerLayout->addWidget(uploadButton_);

    openButton_ = new QPushButton("Open Certificate", this);
    connect(openButton_, &QPushButton::clicked, this, &MyCertificationsWidget::onOpenClicked);
    headerLayout->addWidget(openButton_);

    refreshButton_ = new QPushButton("Refresh", this);
    connect(refreshButton_, &QPushButton::clicked, this, &MyCertificationsWidget::onRefreshClicked);
    headerLayout->addWidget(refreshButton_);
    mainLayout->addLayout(headerLayout);

    summaryLabel_ = new QLabel("Loading certifications...", this);
    summaryLabel_->setStyleSheet("QLabel { color: #5f6b7a; font-weight: 600; }");
    mainLayout->addWidget(summaryLabel_);

    certificationsTable_ = new QTableWidget(this);
    certificationsTable_->setColumnCount(5);
    certificationsTable_->setHorizontalHeaderLabels({"Certification", "Date Earned", "Expiry Date", "Status", "Certificate"});
    certificationsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    certificationsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    certificationsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    certificationsTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    certificationsTable_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    certificationsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    certificationsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    certificationsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    certificationsTable_->setAlternatingRowColors(true);
    mainLayout->addWidget(certificationsTable_);

    Session* session = Application::instance().session();
    const bool canUpload = session && session->isAdmin();
    uploadButton_->setEnabled(canUpload);
    uploadButton_->setToolTip(canUpload ? "Upload a certificate file for this record" : "Manager-only: engineers can view certificates but cannot upload");
}

void MyCertificationsWidget::loadCertifications()
{
    certificationsTable_->setRowCount(0);
    QList<Certification> certifications = certificationRepo_.findByEngineer(engineerId_);

    certificationsTable_->setRowCount(certifications.size());

    int validCount = 0;
    int expiringSoonCount = 0;
    int expiredCount = 0;
    const QDate today = QDate::currentDate();

    for (int i = 0; i < certifications.size(); ++i) {
        const Certification& cert = certifications[i];
        certificationsTable_->setItem(i, 0, new QTableWidgetItem(cert.name()));
        certificationsTable_->setItem(i, 1, new QTableWidgetItem(cert.dateEarned().toString("yyyy-MM-dd")));

        QString expiryText = cert.expiryDate().isValid()
            ? cert.expiryDate().toString("yyyy-MM-dd")
            : "No expiry";
        certificationsTable_->setItem(i, 2, new QTableWidgetItem(expiryText));

        QString status = "Valid";
        QTableWidgetItem* statusItem = new QTableWidgetItem(status);

        if (!cert.expiryDate().isValid()) {
            validCount++;
        } else if (cert.expiryDate() < today) {
            status = "Expired";
            statusItem->setText(status);
            statusItem->setBackground(QColor(255, 228, 230));
            expiredCount++;
        } else if (today.daysTo(cert.expiryDate()) <= 90) {
            status = "Expiring Soon";
            statusItem->setText(status);
            statusItem->setBackground(QColor(254, 249, 195));
            expiringSoonCount++;
        } else {
            validCount++;
        }

        certificationsTable_->setItem(i, 3, statusItem);

        QTableWidgetItem* fileItem = new QTableWidgetItem(cert.certificateFilePath().isEmpty() ? "Not uploaded" : "Uploaded");
        fileItem->setData(Qt::UserRole, cert.certificateFilePath());
        fileItem->setData(Qt::UserRole + 1, cert.id());
        certificationsTable_->setItem(i, 4, fileItem);
    }

    summaryLabel_->setText(QString("Total: %1 | Valid: %2 | Expiring Soon: %3 | Expired: %4")
        .arg(certifications.size())
        .arg(validCount)
        .arg(expiringSoonCount)
        .arg(expiredCount));

    Logger::instance().info("MyCertificationsWidget",
        QString("Loaded %1 certifications for engineer %2").arg(certifications.size()).arg(engineerId_));
}

void MyCertificationsWidget::onRefreshClicked()
{
    loadCertifications();
}

void MyCertificationsWidget::onUploadClicked()
{
    Session* session = Application::instance().session();
    if (!session || !session->isAdmin()) {
        QMessageBox::warning(this, "Permission Denied", "Only managers can upload certification files.");
        return;
    }

    int currentRow = certificationsTable_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a certification row first.");
        return;
    }

    QTableWidgetItem* fileItem = certificationsTable_->item(currentRow, 4);
    if (!fileItem) {
        return;
    }
    const int certificationId = fileItem->data(Qt::UserRole + 1).toInt();
    const QString certName = certificationsTable_->item(currentRow, 0)->text();

    const QString selectedFile = QFileDialog::getOpenFileName(
        this,
        "Select Certificate File",
        QString(),
        "Certificate Files (*.pdf *.png *.jpg *.jpeg *.doc *.docx);;All Files (*.*)");
    if (selectedFile.isEmpty()) {
        return;
    }

    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseDir.isEmpty()) {
        QMessageBox::critical(this, "Error", "Cannot determine app data directory for storing certificates.");
        return;
    }
    QDir storageDir(baseDir + "/certificates");
    if (!storageDir.exists() && !storageDir.mkpath(".")) {
        QMessageBox::critical(this, "Error", "Failed to create certificate storage directory.");
        return;
    }

    QFileInfo fileInfo(selectedFile);
    QString safeName = fileInfo.fileName();
    safeName.replace(" ", "_");
    QString destinationPath = storageDir.filePath(
        QString("cert_%1_%2_%3")
            .arg(certificationId)
            .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
            .arg(safeName));

    if (!QFile::copy(selectedFile, destinationPath)) {
        QMessageBox::critical(this, "Error", "Failed to copy selected certificate file.");
        return;
    }

    if (!certificationRepo_.updateCertificateFilePath(certificationId, destinationPath)) {
        QFile::remove(destinationPath);
        QMessageBox::critical(this, "Error", "Failed to save certificate path: " + certificationRepo_.lastError());
        return;
    }

    Logger::instance().info("MyCertificationsWidget",
        QString("Uploaded certificate file for certification %1 (%2)").arg(certificationId).arg(certName));
    QMessageBox::information(this, "Success", "Certificate uploaded successfully.");
    loadCertifications();
}

void MyCertificationsWidget::onOpenClicked()
{
    int currentRow = certificationsTable_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a certification row first.");
        return;
    }

    QTableWidgetItem* fileItem = certificationsTable_->item(currentRow, 4);
    if (!fileItem) {
        QMessageBox::warning(this, "No File", "No certificate file is linked to this row.");
        return;
    }

    const QString filePath = fileItem->data(Qt::UserRole).toString();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "No File", "No certificate has been uploaded for this certification yet.");
        return;
    }

    QFileInfo info(filePath);
    if (!info.exists()) {
        QMessageBox::warning(this, "Missing File", "Linked certificate file was not found on disk.");
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(this, "Open Failed", "Could not open the certificate file.");
    }
}
