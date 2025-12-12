#include "AnalyticsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QLabel>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    Logger::instance().info("AnalyticsWidget", "Analytics widget initialized");
}

AnalyticsWidget::~AnalyticsWidget()
{
}

void AnalyticsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Analytics & Charts", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel* infoLabel = new QLabel(
        "Analytics features include:\n\n"
        "- Skill distribution charts\n"
        "- Progress tracking over time\n"
        "- Competency heat maps\n"
        "- Training gap analysis\n\n"
        "[Chart visualization placeholder - requires QtCharts module]",
        this);

    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch();

    setLayout(mainLayout);
}
