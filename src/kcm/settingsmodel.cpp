#include "settingsmodel.h"
#include <KConfigGroup>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

SettingsModel::SettingsModel(QObject *parent)
    : QObject(parent)
    , m_config(KSharedConfig::openConfig(QStringLiteral("plasma-desktop-organizerrc")))
{
    load();
}

void SettingsModel::load()
{
    m_loading = true;

    KConfigGroup general = m_config->group(QStringLiteral("General"));
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));

    // General
    setAutoStart(general.readEntry("AutoStart", true));
    setDefaultIconSize(general.readEntry("DefaultIconSize", 64));
    setShowSystemTray(general.readEntry("ShowSystemTray", true));
    setStartMinimized(general.readEntry("StartMinimized", false));

    // Appearance
    setFenceOpacity(appearance.readEntry("FenceOpacity", 85));
    setFenceBackgroundColor(appearance.readEntry("FenceBackgroundColor", QStringLiteral("#2c2c2c")));
    setTitleBarHeight(appearance.readEntry("TitleBarHeight", 32));
    setSnapToGrid(appearance.readEntry("SnapToGrid", false));
    setGridSize(appearance.readEntry("GridSize", 20));
    setEnableAnimations(appearance.readEntry("EnableAnimations", true));

    // Advanced
    setDataDirectory(advanced.readEntry("DataDirectory", defaultDataDirectory()));
    setClickThroughDelay(advanced.readEntry("ClickThroughDelay", 0));
    setMinFenceWidth(advanced.readEntry("MinFenceWidth", 100));
    setMinFenceHeight(advanced.readEntry("MinFenceHeight", 80));

    m_loading = false;
    setNeedsSave(false);
}

void SettingsModel::save()
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));

    // General
    general.writeEntry("AutoStart", m_autoStart);
    general.writeEntry("DefaultIconSize", m_defaultIconSize);
    general.writeEntry("ShowSystemTray", m_showSystemTray);
    general.writeEntry("StartMinimized", m_startMinimized);

    // Appearance
    appearance.writeEntry("FenceOpacity", m_fenceOpacity);
    appearance.writeEntry("FenceBackgroundColor", m_fenceBackgroundColor);
    appearance.writeEntry("TitleBarHeight", m_titleBarHeight);
    appearance.writeEntry("SnapToGrid", m_snapToGrid);
    appearance.writeEntry("GridSize", m_gridSize);
    appearance.writeEntry("EnableAnimations", m_enableAnimations);

    // Advanced
    advanced.writeEntry("DataDirectory", m_dataDirectory);
    advanced.writeEntry("ClickThroughDelay", m_clickThroughDelay);
    advanced.writeEntry("MinFenceWidth", m_minFenceWidth);
    advanced.writeEntry("MinFenceHeight", m_minFenceHeight);

    m_config->sync();
    updateAutoStartFile();
    setNeedsSave(false);
}

void SettingsModel::defaults()
{
    m_loading = true;
    setAutoStart(true);
    setDefaultIconSize(64);
    setShowSystemTray(true);
    setStartMinimized(false);
    setFenceOpacity(85);
    setFenceBackgroundColor(QStringLiteral("#2c2c2c"));
    setTitleBarHeight(32);
    setSnapToGrid(false);
    setGridSize(20);
    setEnableAnimations(true);
    setDataDirectory(defaultDataDirectory());
    setClickThroughDelay(0);
    setMinFenceWidth(100);
    setMinFenceHeight(80);
    m_loading = false;
    setNeedsSave(true);  // user requested defaults; mark dirty so Apply is available
}

// General setters
void SettingsModel::setAutoStart(bool enabled)
{
    if (m_autoStart != enabled) {
        m_autoStart = enabled;
        Q_EMIT autoStartChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setDefaultIconSize(int size)
{
    if (m_defaultIconSize != size) {
        m_defaultIconSize = size;
        Q_EMIT defaultIconSizeChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setShowSystemTray(bool show)
{
    if (m_showSystemTray != show) {
        m_showSystemTray = show;
        Q_EMIT showSystemTrayChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setStartMinimized(bool minimized)
{
    if (m_startMinimized != minimized) {
        m_startMinimized = minimized;
        Q_EMIT startMinimizedChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

// Appearance setters
void SettingsModel::setFenceOpacity(int opacity)
{
    if (m_fenceOpacity != opacity) {
        m_fenceOpacity = opacity;
        Q_EMIT fenceOpacityChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setFenceBackgroundColor(const QString &color)
{
    if (m_fenceBackgroundColor != color) {
        m_fenceBackgroundColor = color;
        Q_EMIT fenceBackgroundColorChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setTitleBarHeight(int height)
{
    if (m_titleBarHeight != height) {
        m_titleBarHeight = height;
        Q_EMIT titleBarHeightChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setSnapToGrid(bool snap)
{
    if (m_snapToGrid != snap) {
        m_snapToGrid = snap;
        Q_EMIT snapToGridChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setGridSize(int size)
{
    if (m_gridSize != size) {
        m_gridSize = size;
        Q_EMIT gridSizeChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setEnableAnimations(bool enabled)
{
    if (m_enableAnimations != enabled) {
        m_enableAnimations = enabled;
        Q_EMIT enableAnimationsChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

// Advanced setters
void SettingsModel::setDataDirectory(const QString &path)
{
    if (m_dataDirectory != path) {
        m_dataDirectory = path;
        Q_EMIT dataDirectoryChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setClickThroughDelay(int ms)
{
    if (m_clickThroughDelay != ms) {
        m_clickThroughDelay = ms;
        Q_EMIT clickThroughDelayChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setMinFenceWidth(int width)
{
    if (m_minFenceWidth != width) {
        m_minFenceWidth = width;
        Q_EMIT minFenceWidthChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

void SettingsModel::setMinFenceHeight(int height)
{
    if (m_minFenceHeight != height) {
        m_minFenceHeight = height;
        Q_EMIT minFenceHeightChanged();
        if (!m_loading) setNeedsSave(true);
    }
}

// Helper methods
QString SettingsModel::defaultDataDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
           + QStringLiteral("/dev.quinnjr.desktop-organizer");
}

void SettingsModel::setNeedsSave(bool needs)
{
    if (m_needsSave != needs) {
        m_needsSave = needs;
        Q_EMIT needsSaveChanged();
    }
}

void SettingsModel::updateAutoStartFile()
{
    const QString autostartDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
                                 + QStringLiteral("/autostart");
    const QString desktopFile = autostartDir + QStringLiteral("/plasma-organizer.desktop");

    if (m_autoStart) {
        // Ensure directory exists
        QDir().mkpath(autostartDir);

        const QString sourcePath = QStandardPaths::locate(
            QStandardPaths::GenericConfigLocation,
            QStringLiteral("autostart/plasma-organizer.desktop"));
        if (!sourcePath.isEmpty()) {
            QFile::remove(desktopFile);  // Remove existing first (copy won't overwrite)
            if (!QFile::copy(sourcePath, desktopFile)) {
                qWarning() << "SettingsModel: failed to copy autostart file to" << desktopFile;
            }
        } else {
            qWarning() << "SettingsModel: could not locate autostart template plasma-organizer.desktop";
        }
    } else {
        if (!QFile::remove(desktopFile) && QFile::exists(desktopFile)) {
            qWarning() << "SettingsModel: failed to remove autostart file" << desktopFile;
        }
    }
}
