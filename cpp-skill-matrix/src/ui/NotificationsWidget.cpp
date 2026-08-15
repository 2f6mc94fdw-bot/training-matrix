#include "NotificationsWidget.h"

#include "../core/Application.h"
#include "../core/Session.h"
#include "../utils/Logger.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

NotificationsWidget::NotificationsWidget(QWidget* parent)
    : QWidget(parent)
    , table_(nullptr)
    , refreshButton_(nullptr)
    , markReadButton_(nullptr)
    , summaryLabel_(nullptr)
    , typeFilterCombo_(nullptr)
    , searchEdit_(nullptr)
    , unreadOnlyCheck_(nullptr)
{
    setupUI();
    loadNotifications();
    Logger::instance().info("NotificationsWidget", "Notifications widget initialized");
}

NotificationsWidget::~NotificationsWidget() = default;

void NotificationsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    QLabel* titleLabel = new QLabel("Notifications", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(30);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel(
        "Updates from engineer self-assessments (production and core skills).", this);
    subtitleLabel->setStyleSheet("color: #475569;");
    mainLayout->addWidget(subtitleLabel);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    unreadOnlyCheck_ = new QCheckBox("Unread only", this);
    typeFilterCombo_ = new QComboBox(this);
    typeFilterCombo_->addItem("All Types", "");
    typeFilterCombo_->addItem("Manager Review", "manager review");
    typeFilterCombo_->addItem("Core Skills", "core");
    typeFilterCombo_->addItem("Production", "production");
    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Search sender/title/message...");
    searchEdit_->setMinimumWidth(260);
    filterLayout->addWidget(unreadOnlyCheck_);
    filterLayout->addWidget(typeFilterCombo_);
    filterLayout->addWidget(searchEdit_, 1);
    mainLayout->addLayout(filterLayout);

    QHBoxLayout* actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);

    refreshButton_ = new QPushButton("Refresh", this);
    markReadButton_ = new QPushButton("Mark All as Read", this);
    summaryLabel_ = new QLabel(this);
    summaryLabel_->setStyleSheet("color: #475569; font-weight: 700;");

    connect(refreshButton_, &QPushButton::clicked, this, &NotificationsWidget::onRefreshClicked);
    connect(markReadButton_, &QPushButton::clicked, this, &NotificationsWidget::onMarkReadClicked);
    connect(unreadOnlyCheck_, &QCheckBox::toggled, this, &NotificationsWidget::onFiltersChanged);
    connect(typeFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NotificationsWidget::onFiltersChanged);
    connect(searchEdit_, &QLineEdit::textChanged, this, &NotificationsWidget::onFiltersChanged);

    actionsLayout->addWidget(refreshButton_);
    actionsLayout->addWidget(markReadButton_);
    actionsLayout->addStretch();
    actionsLayout->addWidget(summaryLabel_);
    mainLayout->addLayout(actionsLayout);

    table_ = new QTableWidget(this);
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels({"When", "From", "Type", "Message"});
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mainLayout->addWidget(table_);
}

void NotificationsWidget::refresh()
{
    loadNotifications();
}

void NotificationsWidget::onRefreshClicked()
{
    loadNotifications();
}

void NotificationsWidget::onMarkReadClicked()
{
    Session* session = Application::instance().session();
    if (!session) {
        return;
    }

    if (!notificationRepo_.markAllReadForUser(session->userId())) {
        Logger::instance().warning("NotificationsWidget",
            "Failed to mark notifications read: " + notificationRepo_.lastError());
        return;
    }

    loadNotifications();
}

void NotificationsWidget::onFiltersChanged()
{
    applyFilters();
}

void NotificationsWidget::loadNotifications()
{
    Session* session = Application::instance().session();
    if (!session) {
        return;
    }

    allNotifications_ = notificationRepo_.findByRecipientUser(session->userId(), false, 500);
    applyFilters();
}

void NotificationsWidget::applyFilters()
{
    const bool unreadOnly = unreadOnlyCheck_ && unreadOnlyCheck_->isChecked();
    const QString typeFilter = typeFilterCombo_ ? typeFilterCombo_->currentData().toString().trimmed().toLower() : "";
    const QString searchFilter = searchEdit_ ? searchEdit_->text().trimmed().toLower() : "";

    QList<EngineerNotification> filtered;
    filtered.reserve(allNotifications_.size());
    for (const EngineerNotification& note : allNotifications_) {
        if (unreadOnly && note.isRead) {
            continue;
        }

        if (!typeFilter.isEmpty()) {
            const QString title = note.title.toLower();
            const QString message = note.message.toLower();
            if (!title.contains(typeFilter) && !message.contains(typeFilter)) {
                continue;
            }
        }

        if (!searchFilter.isEmpty()) {
            const QString haystack = QString("%1 %2 %3")
                .arg(note.createdByName, note.title, note.message)
                .toLower();
            if (!haystack.contains(searchFilter)) {
                continue;
            }
        }

        filtered.append(note);
    }

    table_->setRowCount(filtered.size());

    int unreadCount = 0;
    for (int i = 0; i < filtered.size(); ++i) {
        const EngineerNotification& note = filtered[i];
        if (!note.isRead) {
            unreadCount++;
        }

        table_->setItem(i, 0, new QTableWidgetItem(note.createdAt.toString("dd MMM yyyy HH:mm")));
        table_->setItem(i, 1, new QTableWidgetItem(note.createdByName.isEmpty() ? "Engineer" : note.createdByName));
        table_->setItem(i, 2, new QTableWidgetItem(note.title));
        table_->setItem(i, 3, new QTableWidgetItem(note.message));
    }

    summaryLabel_->setText(QString("%1 shown | %2 unread")
        .arg(filtered.size())
        .arg(unreadCount));
    markReadButton_->setEnabled(unreadCount > 0);
}
