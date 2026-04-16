#pragma once
#include <QObject>
#include <KSharedConfig>

class SettingsModel : public QObject {
    Q_OBJECT

    // General settings
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(int defaultIconSize READ defaultIconSize WRITE setDefaultIconSize NOTIFY defaultIconSizeChanged)
    Q_PROPERTY(bool showSystemTray READ showSystemTray WRITE setShowSystemTray NOTIFY showSystemTrayChanged)
    Q_PROPERTY(bool startMinimized READ startMinimized WRITE setStartMinimized NOTIFY startMinimizedChanged)

    // Appearance settings
    Q_PROPERTY(int fenceOpacity READ fenceOpacity WRITE setFenceOpacity NOTIFY fenceOpacityChanged)
    Q_PROPERTY(QString fenceBackgroundColor READ fenceBackgroundColor WRITE setFenceBackgroundColor NOTIFY fenceBackgroundColorChanged)
    Q_PROPERTY(int titleBarHeight READ titleBarHeight WRITE setTitleBarHeight NOTIFY titleBarHeightChanged)
    Q_PROPERTY(bool snapToGrid READ snapToGrid WRITE setSnapToGrid NOTIFY snapToGridChanged)
    Q_PROPERTY(int gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged)
    Q_PROPERTY(bool enableAnimations READ enableAnimations WRITE setEnableAnimations NOTIFY enableAnimationsChanged)

    // Advanced settings
    Q_PROPERTY(QString dataDirectory READ dataDirectory WRITE setDataDirectory NOTIFY dataDirectoryChanged)
    Q_PROPERTY(int clickThroughDelay READ clickThroughDelay WRITE setClickThroughDelay NOTIFY clickThroughDelayChanged)
    Q_PROPERTY(int minFenceWidth READ minFenceWidth WRITE setMinFenceWidth NOTIFY minFenceWidthChanged)
    Q_PROPERTY(int minFenceHeight READ minFenceHeight WRITE setMinFenceHeight NOTIFY minFenceHeightChanged)

    // Meta
    Q_PROPERTY(bool needsSave READ needsSave NOTIFY needsSaveChanged)

public:
    explicit SettingsModel(QObject *parent = nullptr);

    // General
    bool autoStart() const { return m_autoStart; }
    void setAutoStart(bool enabled);

    int defaultIconSize() const { return m_defaultIconSize; }
    void setDefaultIconSize(int size);

    bool showSystemTray() const { return m_showSystemTray; }
    void setShowSystemTray(bool show);

    bool startMinimized() const { return m_startMinimized; }
    void setStartMinimized(bool minimized);

    // Appearance
    int fenceOpacity() const { return m_fenceOpacity; }
    void setFenceOpacity(int opacity);

    QString fenceBackgroundColor() const { return m_fenceBackgroundColor; }
    void setFenceBackgroundColor(const QString &color);

    int titleBarHeight() const { return m_titleBarHeight; }
    void setTitleBarHeight(int height);

    bool snapToGrid() const { return m_snapToGrid; }
    void setSnapToGrid(bool snap);

    int gridSize() const { return m_gridSize; }
    void setGridSize(int size);

    bool enableAnimations() const { return m_enableAnimations; }
    void setEnableAnimations(bool enabled);

    // Advanced
    QString dataDirectory() const { return m_dataDirectory; }
    void setDataDirectory(const QString &path);

    int clickThroughDelay() const { return m_clickThroughDelay; }
    void setClickThroughDelay(int ms);

    int minFenceWidth() const { return m_minFenceWidth; }
    void setMinFenceWidth(int width);

    int minFenceHeight() const { return m_minFenceHeight; }
    void setMinFenceHeight(int height);

    // Meta
    bool needsSave() const { return m_needsSave; }

public Q_SLOTS:
    void load();
    void save();
    void defaults();

Q_SIGNALS:
    void autoStartChanged();
    void defaultIconSizeChanged();
    void showSystemTrayChanged();
    void startMinimizedChanged();
    void fenceOpacityChanged();
    void fenceBackgroundColorChanged();
    void titleBarHeightChanged();
    void snapToGridChanged();
    void gridSizeChanged();
    void enableAnimationsChanged();
    void dataDirectoryChanged();
    void clickThroughDelayChanged();
    void minFenceWidthChanged();
    void minFenceHeightChanged();
    void needsSaveChanged();

private:
    void setNeedsSave(bool needs);
    void updateAutoStartFile();

    KSharedConfig::Ptr m_config;

    // Cached values
    bool m_autoStart = true;
    int m_defaultIconSize = 64;
    bool m_showSystemTray = true;
    bool m_startMinimized = false;
    int m_fenceOpacity = 85;
    QString m_fenceBackgroundColor = QStringLiteral("#2c2c2c");
    int m_titleBarHeight = 32;
    bool m_snapToGrid = false;
    int m_gridSize = 20;
    bool m_enableAnimations = true;
    QString m_dataDirectory;
    int m_clickThroughDelay = 0;
    int m_minFenceWidth = 100;
    int m_minFenceHeight = 80;

    bool m_needsSave = false;
    bool m_loading = false;

    static QString defaultDataDirectory();
};
