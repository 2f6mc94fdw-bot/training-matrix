#include "IconProvider.h"
#include "Logger.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QPixmap>
#include <QFont>
#include <QBrush>
#include <QPen>

IconProvider& IconProvider::instance()
{
    static IconProvider instance;
    return instance;
}

IconProvider::IconProvider(QObject* parent)
    : QObject(parent)
{
}

IconProvider::~IconProvider()
{
}

void IconProvider::initialize()
{
    loadIcons();

    // Map names to types for string-based lookup
    nameToType_["add"] = Add;
    nameToType_["edit"] = Edit;
    nameToType_["delete"] = Delete;
    nameToType_["save"] = Save;
    nameToType_["cancel"] = Cancel;
    nameToType_["refresh"] = Refresh;
    nameToType_["search"] = Search;
    nameToType_["filter"] = Filter;
    nameToType_["export"] = Export;
    nameToType_["import"] = Import;
    nameToType_["print"] = Print;
    nameToType_["engineer"] = Engineer;
    nameToType_["production_area"] = ProductionArea;
    nameToType_["machine"] = Machine;
    nameToType_["competency"] = Competency;
    nameToType_["assessment"] = Assessment;
    nameToType_["certification"] = Certification;
    nameToType_["core_skill"] = CoreSkill;
    nameToType_["success"] = Success;
    nameToType_["warning"] = Warning;
    nameToType_["error"] = Error;
    nameToType_["info"] = Info;
    nameToType_["help"] = Help;
    nameToType_["dashboard"] = Dashboard;
    nameToType_["reports"] = Reports;
    nameToType_["analytics"] = Analytics;
    nameToType_["settings"] = Settings;
    nameToType_["logout"] = Logout;
    nameToType_["snapshot"] = Snapshot;
    nameToType_["audit_log"] = AuditLog;
    nameToType_["calendar"] = Calendar;
    nameToType_["chart"] = Chart;
    nameToType_["document"] = Document;
    nameToType_["folder"] = Folder;
    nameToType_["file"] = File;
    nameToType_["user"] = User;
    nameToType_["users"] = Users;

    Logger::instance().info("IconProvider", "Icon provider initialized");
}

QIcon IconProvider::getIcon(IconType type) const
{
    return icons_.value(type, QIcon());
}

QIcon IconProvider::getIconByName(const QString& name) const
{
    IconType type = nameToType_.value(name, Help);
    return getIcon(type);
}

QIcon IconProvider::createColorIcon(const QColor& color, int size) const
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(2, 2, size - 4, size - 4, 2, 2);

    return QIcon(pixmap);
}

QIcon IconProvider::createShapeIcon(const QString& shape, const QColor& color, int size) const
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(color.darker(120), 1));

    if (shape == "circle") {
        painter.drawEllipse(2, 2, size - 4, size - 4);
    } else if (shape == "square") {
        painter.drawRect(2, 2, size - 4, size - 4);
    } else if (shape == "triangle") {
        QPolygon triangle;
        triangle << QPoint(size / 2, 2)
                 << QPoint(size - 2, size - 2)
                 << QPoint(2, size - 2);
        painter.drawPolygon(triangle);
    } else if (shape == "star") {
        // Simple star shape
        painter.drawEllipse(size / 2 - 3, size / 2 - 3, 6, 6);
    }

    return QIcon(pixmap);
}

void IconProvider::loadIcons()
{
    QStyle* style = qApp ? qApp->style() : nullptr;

    if (style) {
        // Actions
        icons_[Add] = style->standardIcon(QStyle::SP_FileIcon);
        icons_[Edit] = style->standardIcon(QStyle::SP_FileDialogDetailedView);
        icons_[Delete] = style->standardIcon(QStyle::SP_TrashIcon);
        icons_[Save] = style->standardIcon(QStyle::SP_DialogSaveButton);
        icons_[Cancel] = style->standardIcon(QStyle::SP_DialogCancelButton);
        icons_[Refresh] = style->standardIcon(QStyle::SP_BrowserReload);
        icons_[Search] = createTextIcon("🔍", QColor("#2196F3"), 20);
        icons_[Filter] = createTextIcon("⚡", QColor("#FF9800"), 20);
        icons_[Export] = style->standardIcon(QStyle::SP_DialogSaveButton);
        icons_[Import] = style->standardIcon(QStyle::SP_DialogOpenButton);
        icons_[Print] = style->standardIcon(QStyle::SP_DialogSaveButton);

        // Navigation
        icons_[Back] = style->standardIcon(QStyle::SP_ArrowBack);
        icons_[Forward] = style->standardIcon(QStyle::SP_ArrowForward);
        icons_[Up] = style->standardIcon(QStyle::SP_ArrowUp);
        icons_[Down] = style->standardIcon(QStyle::SP_ArrowDown);
        icons_[Left] = style->standardIcon(QStyle::SP_ArrowLeft);
        icons_[Right] = style->standardIcon(QStyle::SP_ArrowRight);

        // Data - use colored icons for better visual distinction
        icons_[Engineer] = createTextIcon("👤", QColor("#2196F3"), 20);
        icons_[ProductionArea] = createTextIcon("🏭", QColor("#9C27B0"), 20);
        icons_[Machine] = createTextIcon("⚙️", QColor("#FF9800"), 20);
        icons_[Competency] = createTextIcon("✓", QColor("#4CAF50"), 20);
        icons_[Assessment] = createTextIcon("📋", QColor("#00BCD4"), 20);
        icons_[Certification] = createTextIcon("🏆", QColor("#FFC107"), 20);
        icons_[CoreSkill] = createTextIcon("💡", QColor("#E91E63"), 20);

        // Status
        icons_[Success] = createColorIcon(QColor("#4CAF50"), 16);
        icons_[Warning] = createColorIcon(QColor("#FF9800"), 16);
        icons_[Error] = createColorIcon(QColor("#F44336"), 16);
        icons_[Info] = style->standardIcon(QStyle::SP_MessageBoxInformation);
        icons_[Help] = style->standardIcon(QStyle::SP_MessageBoxQuestion);

        // UI
        icons_[Dashboard] = createTextIcon("📊", QColor("#2196F3"), 20);
        icons_[Reports] = createTextIcon("📄", QColor("#4CAF50"), 20);
        icons_[Analytics] = createTextIcon("📈", QColor("#9C27B0"), 20);
        icons_[Settings] = style->standardIcon(QStyle::SP_FileDialogDetailedView);
        icons_[Logout] = createTextIcon("⎋", QColor("#F44336"), 20);
        icons_[Snapshot] = createTextIcon("📷", QColor("#00BCD4"), 20);
        icons_[AuditLog] = createTextIcon("📝", QColor("#FF9800"), 20);

        // Misc
        icons_[Calendar] = createTextIcon("📅", QColor("#4CAF50"), 20);
        icons_[Chart] = createTextIcon("📊", QColor("#9C27B0"), 20);
        icons_[Document] = style->standardIcon(QStyle::SP_FileIcon);
        icons_[Folder] = style->standardIcon(QStyle::SP_DirIcon);
        icons_[File] = style->standardIcon(QStyle::SP_FileIcon);
        icons_[User] = createTextIcon("👤", QColor("#2196F3"), 20);
        icons_[Users] = createTextIcon("👥", QColor("#2196F3"), 20);
        icons_[Lock] = createTextIcon("🔒", QColor("#F44336"), 20);
        icons_[Unlock] = createTextIcon("🔓", QColor("#4CAF50"), 20);
    } else {
        Logger::instance().warning("IconProvider", "QApplication style not available");
    }
}

QIcon IconProvider::createFromStandardPixmap(QStyle::StandardPixmap pixmap) const
{
    if (qApp && qApp->style()) {
        return qApp->style()->standardIcon(pixmap);
    }
    return QIcon();
}

QIcon IconProvider::createTextIcon(const QString& text, const QColor& color, int size) const
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Draw background circle
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);

    // Draw text
    QFont font = painter.font();
    font.setPixelSize(size / 2);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QPen(QColor(255, 255, 255)));

    painter.drawText(QRect(0, 0, size, size), Qt::AlignCenter, text);

    return QIcon(pixmap);
}
