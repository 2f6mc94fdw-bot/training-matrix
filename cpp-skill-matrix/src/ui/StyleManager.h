#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QMap>

/**
 * @brief Manages application styling and theming
 *
 * Provides a centralized system for managing application styles,
 * colors, and themes (light and dark mode).
 */
class StyleManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to StyleManager instance
     */
    static StyleManager& instance();

    /**
     * @brief Theme enumeration
     */
    enum Theme {
        Light,
        Dark
    };

    /**
     * @brief Initialize the style manager
     */
    void initialize();

    /**
     * @brief Apply a theme to the application
     * @param theme Theme to apply
     */
    void applyTheme(Theme theme);

    /**
     * @brief Get current theme
     * @return Current theme
     */
    Theme currentTheme() const { return currentTheme_; }

    /**
     * @brief Get complete stylesheet for current theme
     * @return Stylesheet string
     */
    QString getStylesheet() const;

    /**
     * @brief Get color for a specific role
     * @param colorName Color name (e.g., "primary", "secondary")
     * @return QColor object
     */
    QColor getColor(const QString& colorName) const;

    /**
     * @brief Get button style
     * @param type Button type ("primary", "secondary", "danger")
     * @return Button stylesheet
     */
    QString getButtonStyle(const QString& type = "primary") const;

    /**
     * @brief Get table style
     * @return Table stylesheet
     */
    QString getTableStyle() const;

    /**
     * @brief Get card/group box style
     * @return Card stylesheet
     */
    QString getCardStyle() const;

    /**
     * @brief Get input field style
     * @return Input field stylesheet
     */
    QString getInputStyle() const;

signals:
    /**
     * @brief Emitted when theme changes
     * @param theme New theme
     */
    void themeChanged(Theme theme);

private:
    // Singleton - private constructor
    explicit StyleManager(QObject* parent = nullptr);
    ~StyleManager();

    // Delete copy constructor and assignment operator
    StyleManager(const StyleManager&) = delete;
    StyleManager& operator=(const StyleManager&) = delete;

    /**
     * @brief Build stylesheet for specified theme
     * @param theme Theme to build stylesheet for
     * @return Complete stylesheet string
     */
    QString buildStylesheet(Theme theme) const;

    /**
     * @brief Initialize color schemes
     */
    void initializeColors();

    /**
     * @brief Get base colors for theme
     * @param theme Theme
     * @return Map of color names to QColor objects
     */
    QMap<QString, QColor> getThemeColors(Theme theme) const;

private:
    Theme currentTheme_;
    QMap<QString, QColor> lightColors_;
    QMap<QString, QColor> darkColors_;
    QString currentStylesheet_;
};

#endif // STYLEMANAGER_H
