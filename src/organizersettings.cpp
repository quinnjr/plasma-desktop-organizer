#include "organizersettings.h"
#include <KConfigGroup>
#include <QStandardPaths>

OrganizerSettings *OrganizerSettings::s_instance = nullptr;

OrganizerSettings::OrganizerSettings(QObject *parent)
    : QObject(parent)
    , m_config(KSharedConfig::openConfig(QStringLiteral("plasma-desktop-organizerrc")))
{
    Q_ASSERT_X(!s_instance, "OrganizerSettings", "Singleton already exists");
    s_instance = this;
}

OrganizerSettings::~OrganizerSettings()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

OrganizerSettings *OrganizerSettings::instance()
{
    Q_ASSERT_X(s_instance, "OrganizerSettings::instance",
               "Singleton not constructed. Must create OrganizerSettings before calling instance()");
    return s_instance;
}

// General settings
bool OrganizerSettings::autoStart() const
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    return general.readEntry("AutoStart", true);
}

void OrganizerSettings::setAutoStart(bool enabled)
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    general.writeEntry("AutoStart", enabled);
}

int OrganizerSettings::defaultIconSize() const
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    return general.readEntry("DefaultIconSize", 64);
}

void OrganizerSettings::setDefaultIconSize(int size)
{
    if (size < 16 || size > 256) {
        qWarning() << "Invalid icon size:" << size << "(must be 16-256), clamping";
        size = qBound(16, size, 256);
    }
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    general.writeEntry("DefaultIconSize", size);
}

bool OrganizerSettings::showSystemTray() const
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    return general.readEntry("ShowSystemTray", true);
}

void OrganizerSettings::setShowSystemTray(bool show)
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    general.writeEntry("ShowSystemTray", show);
}

bool OrganizerSettings::startMinimized() const
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    return general.readEntry("StartMinimized", false);
}

void OrganizerSettings::setStartMinimized(bool minimized)
{
    KConfigGroup general = m_config->group(QStringLiteral("General"));
    general.writeEntry("StartMinimized", minimized);
}

// Appearance settings
int OrganizerSettings::fenceOpacity() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("FenceOpacity", 85);
}

void OrganizerSettings::setFenceOpacity(int opacity)
{
    if (opacity < 0 || opacity > 100) {
        qWarning() << "Invalid fence opacity:" << opacity << "(must be 0-100), clamping";
        opacity = qBound(0, opacity, 100);
    }
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("FenceOpacity", opacity);
}

QString OrganizerSettings::fenceBackgroundColor() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("FenceBackgroundColor", QStringLiteral("#2c2c2c"));
}

void OrganizerSettings::setFenceBackgroundColor(const QString &color)
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("FenceBackgroundColor", color);
}

int OrganizerSettings::titleBarHeight() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("TitleBarHeight", 32);
}

void OrganizerSettings::setTitleBarHeight(int height)
{
    if (height < 16 || height > 100) {
        qWarning() << "Invalid title bar height:" << height << "(must be 16-100), clamping";
        height = qBound(16, height, 100);
    }
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("TitleBarHeight", height);
}

int OrganizerSettings::resizeHandleSize() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("ResizeHandleSize", 16);
}

void OrganizerSettings::setResizeHandleSize(int size)
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("ResizeHandleSize", size);
}

bool OrganizerSettings::snapToGrid() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("SnapToGrid", false);
}

void OrganizerSettings::setSnapToGrid(bool snap)
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("SnapToGrid", snap);
}

int OrganizerSettings::gridSize() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("GridSize", 20);
}

void OrganizerSettings::setGridSize(int size)
{
    if (size < 5 || size > 100) {
        qWarning() << "Invalid grid size:" << size << "(must be 5-100), clamping";
        size = qBound(5, size, 100);
    }
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("GridSize", size);
}

bool OrganizerSettings::enableAnimations() const
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    return appearance.readEntry("EnableAnimations", true);
}

void OrganizerSettings::setEnableAnimations(bool enabled)
{
    KConfigGroup appearance = m_config->group(QStringLiteral("Appearance"));
    appearance.writeEntry("EnableAnimations", enabled);
}

// Advanced settings
QString OrganizerSettings::dataDirectory() const
{
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    const QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                                + QStringLiteral("/dev.quinnjr.desktop-organizer");
    return advanced.readEntry("DataDirectory", defaultPath);
}

void OrganizerSettings::setDataDirectory(const QString &path)
{
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    advanced.writeEntry("DataDirectory", path);
}

int OrganizerSettings::clickThroughDelay() const
{
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    return advanced.readEntry("ClickThroughDelay", 0);
}

void OrganizerSettings::setClickThroughDelay(int ms)
{
    if (ms < 0 || ms > 5000) {
        qWarning() << "Invalid click-through delay:" << ms << "(must be 0-5000ms), clamping";
        ms = qBound(0, ms, 5000);
    }
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    advanced.writeEntry("ClickThroughDelay", ms);
}

int OrganizerSettings::minFenceWidth() const
{
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    return advanced.readEntry("MinFenceWidth", 100);
}

void OrganizerSettings::setMinFenceWidth(int width)
{
    if (width < 50 || width > 500) {
        qWarning() << "Invalid minimum fence width:" << width << "(must be 50-500), clamping";
        width = qBound(50, width, 500);
    }
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    advanced.writeEntry("MinFenceWidth", width);
}

int OrganizerSettings::minFenceHeight() const
{
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    return advanced.readEntry("MinFenceHeight", 80);
}

void OrganizerSettings::setMinFenceHeight(int height)
{
    if (height < 40 || height > 400) {
        qWarning() << "Invalid minimum fence height:" << height << "(must be 40-400), clamping";
        height = qBound(40, height, 400);
    }
    KConfigGroup advanced = m_config->group(QStringLiteral("Advanced"));
    advanced.writeEntry("MinFenceHeight", height);
}

void OrganizerSettings::sync()
{
    m_config->sync();
    Q_EMIT settingsChanged();
}

void OrganizerSettings::reload()
{
    m_config->reparseConfiguration();
    Q_EMIT settingsChanged();
}
