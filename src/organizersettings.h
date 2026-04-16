#pragma once
#include <QObject>
#include <QString>
#include <KSharedConfig>

/**
 * @class OrganizerSettings
 * @brief Singleton configuration manager for Plasma Desktop Organizer
 *
 * Wraps KConfig access for application settings stored at:
 * ~/.config/plasma-desktop-organizerrc
 *
 * Usage:
 * @code
 * OrganizerSettings::instance()->setAutoStart(true);
 * OrganizerSettings::instance()->sync();
 * @endcode
 *
 * @note Must be constructed before first instance() call
 * @note NOT thread-safe; use from main thread only
 */
class OrganizerSettings : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OrganizerSettings)
public:
    static OrganizerSettings *instance();

    // General settings
    bool autoStart() const;
    void setAutoStart(bool enabled);

    int defaultIconSize() const;
    void setDefaultIconSize(int size);

    bool showSystemTray() const;
    void setShowSystemTray(bool show);

    bool startMinimized() const;
    void setStartMinimized(bool minimized);

    // Appearance settings
    int fenceOpacity() const;
    void setFenceOpacity(int opacity);

    QString fenceBackgroundColor() const;
    void setFenceBackgroundColor(const QString &color);

    int titleBarHeight() const;
    void setTitleBarHeight(int height);

    int resizeHandleSize() const;
    void setResizeHandleSize(int size);

    bool snapToGrid() const;
    void setSnapToGrid(bool snap);

    int gridSize() const;
    void setGridSize(int size);

    bool enableAnimations() const;
    void setEnableAnimations(bool enabled);

    // Advanced settings
    QString dataDirectory() const;
    void setDataDirectory(const QString &path);

    int clickThroughDelay() const;
    void setClickThroughDelay(int ms);

    int minFenceWidth() const;
    void setMinFenceWidth(int width);

    int minFenceHeight() const;
    void setMinFenceHeight(int height);

    void sync();
    void reload();

Q_SIGNALS:
    void settingsChanged();

private:
    explicit OrganizerSettings(QObject *parent = nullptr);
    ~OrganizerSettings() override;

    KSharedConfig::Ptr m_config;
    static OrganizerSettings *s_instance;

    friend class TestOrganizerSettings;
    friend class TestFenceManager;
    friend class TestFenceModel;
};
