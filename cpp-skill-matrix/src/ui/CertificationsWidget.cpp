#include "CertificationsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QHash>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QUrl>

CertificationsWidget::CertificationsWidget(QWidget* parent)
    : QWidget(parent)
    , engineerFilterCombo_(nullptr)
    , certificationsTable_(nullptr)
    , addButton_(nullptr)
    , deleteButton_(nullptr)
    , uploadButton_(nullptr)
    , openButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadEngineers();
    loadCertifications();
    Logger::instance().info("CertificationsWidget", "Certifications widget initialized");
}

CertificationsWidget::~CertificationsWidget()
{
}

void CertificationsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel("Certifications Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Filter
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Filter by Engineer:", this);
    engineerFilterCombo_ = new QComboBox(this);
    connect(engineerFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CertificationsWidget::onEngineerFilterChanged);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(engineerFilterCombo_);
    filterLayout->addStretch();

    mainLayout->addLayout(filterLayout);

    // Table
    certificationsTable_ = new QTableWidget(this);
    certificationsTable_->setColumnCount(7);
    certificationsTable_->setHorizontalHeaderLabels({"ID", "Engineer", "Certification Name", "Date Earned", "Expiry Date", "Status", "Certificate"});
    certificationsTable_->horizontalHeader()->setStretchLastSection(true);
    certificationsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    certificationsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    certificationsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    certificationsTable_->setAlternatingRowColors(true);
    certificationsTable_->hideColumn(0); // Hide ID

    mainLayout->addWidget(certificationsTable_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    addButton_ = new QPushButton("Add Certification", this);
    deleteButton_ = new QPushButton("Delete Certification", this);
    uploadButton_ = new QPushButton("Upload Certificate", this);
    openButton_ = new QPushButton("Open Certificate", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(addButton_, &QPushButton::clicked, this, &CertificationsWidget::onAddClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &CertificationsWidget::onDeleteClicked);
    connect(uploadButton_, &QPushButton::clicked, this, &CertificationsWidget::onUploadClicked);
    connect(openButton_, &QPushButton::clicked, this, &CertificationsWidget::onOpenClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &CertificationsWidget::onRefreshClicked);

    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addWidget(uploadButton_);
    buttonLayout->addWidget(openButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void CertificationsWidget::loadEngineers()
{
    engineerFilterCombo_->clear();
    engineerFilterCombo_->addItem("-- All Engineers --", "");

    QList<Engineer> engineers = engineerRepo_.findAll();

    for (const Engineer& engineer : engineers) {
        engineerFilterCombo_->addItem(engineer.name(), engineer.id());
    }
}

void CertificationsWidget::loadCertifications()
{
    certificationsTable_->setRowCount(0);

    QString filterEngineerId = engineerFilterCombo_->currentData().toString();
    QHash<QString, QString> engineerNamesById;
    const QList<Engineer> engineers = engineerRepo_.findAll();
    for (const Engineer& engineer : engineers) {
        engineerNamesById.insert(engineer.id(), engineer.name());
    }

    QList<Certification> certifications;
    if (filterEngineerId.isEmpty()) {
        certifications = certificationRepo_.findAll();
    } else {
        certifications = certificationRepo_.findByEngineer(filterEngineerId);
    }

    certificationsTable_->setRowCount(certifications.size());

    for (int i = 0; i < certifications.size(); ++i) {
        const Certification& cert = certifications[i];
        const QString engineerName = engineerNamesById.value(cert.engineerId(), cert.engineerId());

        certificationsTable_->setItem(i, 0, new QTableWidgetItem(QString::number(cert.id())));
        certificationsTable_->setItem(i, 1, new QTableWidgetItem(engineerName));
        certificationsTable_->setItem(i, 2, new QTableWidgetItem(cert.name()));
        certificationsTable_->setItem(i, 3, new QTableWidgetItem(cert.dateEarned().toString("yyyy-MM-dd")));
        certificationsTable_->setItem(i, 4, new QTableWidgetItem(cert.expiryDate().toString("yyyy-MM-dd")));

        QString status = cert.isExpired() ? "EXPIRED" :
                        (cert.daysUntilExpiry() < 30 ? "EXPIRING SOON" : "Valid");
        QTableWidgetItem* statusItem = new QTableWidgetItem(status);

        if (cert.isExpired()) {
            statusItem->setBackground(QColor(255, 200, 200));
        } else if (cert.daysUntilExpiry() < 30) {
            statusItem->setBackground(QColor(255, 255, 200));
        }

        certificationsTable_->setItem(i, 5, statusItem);

        QTableWidgetItem* fileItem = new QTableWidgetItem(cert.certificateFilePath().isEmpty() ? "Not uploaded" : "Uploaded");
        fileItem->setData(Qt::UserRole, cert.certificateFilePath());
        certificationsTable_->setItem(i, 6, fileItem);
    }

    Logger::instance().info("CertificationsWidget", QString("Loaded %1 certifications").arg(certifications.size()));
}

void CertificationsWidget::showCertificationDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Certification");
    dialog.resize(400, 250);

    QFormLayout* formLayout = new QFormLayout();

    QComboBox* engineerCombo = new QComboBox(&dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    QDateEdit* dateEarnedEdit = new QDateEdit(&dialog);
    QDateEdit* expiryDateEdit = new QDateEdit(&dialog);

    dateEarnedEdit->setDate(QDate::currentDate());
    dateEarnedEdit->setCalendarPopup(true);
    expiryDateEdit->setDate(QDate::currentDate().addYears(1));
    expiryDateEdit->setCalendarPopup(true);

    QList<Engineer> engineers = engineerRepo_.findAll();
    for (const Engineer& engineer : engineers) {
        engineerCombo->addItem(engineer.name(), engineer.id());
    }

    formLayout->addRow("Engineer:", engineerCombo);
    formLayout->addRow("Certification Name:", nameEdit);
    formLayout->addRow("Date Earned:", dateEarnedEdit);
    formLayout->addRow("Expiry Date:", expiryDateEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString engineerId = engineerCombo->currentData().toString();
        QString name = nameEdit->text().trimmed();
        QDate dateEarned = dateEarnedEdit->date();
        QDate expiryDate = expiryDateEdit->date();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Certification name is required.");
            return;
        }

        Certification cert;
        cert.setEngineerId(engineerId);
        cert.setName(name);
        cert.setDateEarned(dateEarned);
        cert.setExpiryDate(expiryDate);

        if (certificationRepo_.save(cert)) {
            Logger::instance().info("CertificationsWidget", "Created certification: " + name);
            QMessageBox::information(this, "Success", "Certification created successfully.");
            loadCertifications();
        } else {
            Logger::instance().error("CertificationsWidget", "Failed to save certification: " + certificationRepo_.lastError());
            QMessageBox::critical(this, "Error", "Failed to save certification: " + certificationRepo_.lastError());
        }
    }
}

void CertificationsWidget::onEngineerFilterChanged(int index)
{
    loadCertifications();
}

void CertificationsWidget::onAddClicked()
{
    showCertificationDialog();
}

void CertificationsWidget::onDeleteClicked()
{
    int currentRow = certificationsTable_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a certification to delete.");
        return;
    }

    int id = certificationsTable_->item(currentRow, 0)->text().toInt();
    QString name = certificationsTable_->item(currentRow, 2)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete certification '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (certificationRepo_.remove(id)) {
            Logger::instance().info("CertificationsWidget", "Deleted certification: " + name);
            QMessageBox::information(this, "Success", "Certification deleted successfully.");
            loadCertifications();
        } else {
            Logger::instance().error("CertificationsWidget", "Failed to delete certification: " + certificationRepo_.lastError());
            QMessageBox::critical(this, "Error", "Failed to delete certification: " + certificationRepo_.lastError());
        }
    }
}

void CertificationsWidget::onRefreshClicked()
{
    loadEngineers();
    loadCertifications();
}

void CertificationsWidget::onUploadClicked()
{
    int currentRow = certificationsTable_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a certification row first.");
        return;
    }

    int id = certificationsTable_->item(currentRow, 0)->text().toInt();
    const QString certName = certificationsTable_->item(currentRow, 2)->text();

    QString selectedFile = QFileDialog::getOpenFileName(
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
            .arg(id)
            .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
            .arg(safeName));

    if (!QFile::copy(selectedFile, destinationPath)) {
        QMessageBox::critical(this, "Error", "Failed to copy selected certificate file.");
        return;
    }

    if (!certificationRepo_.updateCertificateFilePath(id, destinationPath)) {
        QFile::remove(destinationPath);
        QMessageBox::critical(this, "Error", "Failed to save certificate path: " + certificationRepo_.lastError());
        return;
    }

    Logger::instance().info("CertificationsWidget",
        QString("Uploaded certificate file for certification %1 (%2)").arg(id).arg(certName));
    QMessageBox::information(this, "Success", "Certificate uploaded successfully.");
    loadCertifications();
}

void CertificationsWidget::onOpenClicked()
{
    int currentRow = certificationsTable_->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a certification row first.");
        return;
    }

    QTableWidgetItem* fileItem = certificationsTable_->item(currentRow, 6);
    if (!fileItem) {
        QMessageBox::warning(this, "No File", "No certificate file is linked to this row.");
        return;
    }

    QString filePath = fileItem->data(Qt::UserRole).toString();
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
