#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QObject>
#include <QIcon>
#include <QPixmap>
#include <QMap>
#include <QString>

/**
 * @brief Provides icons for the application
 *
 * Centralized icon management using Qt's standard pixmaps
 * and custom colored icons.
 */
class IconProvider : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to IconProvider instance
     */
    static IconProvider& instance();

    /**
     * @brief Icon type enumeration
     */
    enum IconType {
        // Actions
        Add,
        Edit,
        Delete,
        Save,
        Cancel,
        Refresh,
        Search,
        Filter,
        Export,
        Import,
        Print,

        // Navigation
        Back,
        Forward,
        Up,
        Down,
        Left,
        Right,

        // Data
        Engineer,
        ProductionArea,
        Machine,
        Competency,
        Assessment,
        Certification,
        CoreSkill,

        // Status
        Success,
        Warning,
        Error,
        Info,
        Help,

        // UI
        Dashboard,
        Reports,
        Analytics,
        Settings,
        Logout,
        Snapshot,
        AuditLog,

        // Misc
        Calendar,
        Chart,
        Document,
        Folder,
        File,
        User,
        Users,
        Lock,
        Unlock
    };

    /**
     * @brief Initialize the icon provider
     */
    void initialize();

    /**
     * @brief Get icon by type
     * @param type Icon type
     * @return QIcon object
     */
    QIcon getIcon(IconType type) const;

    /**
     * @brief Get icon by name (string)
     * @param name Icon name
     * @return QIcon object
     */
    QIcon getIconByName(const QString& name) const;

    /**
     * @brief Create a colored square icon
     * @param color Icon color
     * @param size Icon size
     * @return QIcon object
     */
    QIcon createColorIcon(const QColor& color, int size = 16) const;

    /**
     * @brief Create a simple shape icon
     * @param shape Shape type ("circle", "square", "triangle")
     * @param color Icon color
     * @param size Icon size
     * @return QIcon object
     */
    QIcon createShapeIcon(const QString& shape, const QColor& color, int size = 16) const;

private:
    // Singleton - private constructor
    explicit IconProvider(QObject* parent = nullptr);
    ~IconProvider();

    // Delete copy constructor and assignment operator
    IconProvider(const IconProvider&) = delete;
    IconProvider& operator=(const IconProvider&) = delete;

    /**
     * @brief Load all icons
     */
    void loadIcons();

    /**
     * @brief Create icon from standard pixmap
     * @param pixmap Qt standard pixmap
     * @return QIcon object
     */
    QIcon createFromStandardPixmap(QStyle::StandardPixmap pixmap) const;

    /**
     * @brief Create icon with text
     * @param text Text to display
     * @param color Background color
     * @param size Icon size
     * @return QIcon object
     */
    QIcon createTextIcon(const QString& text, const QColor& color, int size = 24) const;

private:
    QMap<IconType, QIcon> icons_;
    QMap<QString, IconType> nameToType_;
};

#endif // ICONPROVIDER_H
