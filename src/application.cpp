#include "application.h"
#include "dbusinterface.h"
#include "fencefilemodel.h"
#include "fencemanager.h"
#include "layershellwindow.h"
#include "organizersettings.h"

#include <QScreen>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QtQml>
#include <QDBusConnection>
#include <QDBusError>

Application::Application(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
    setApplicationName(QStringLiteral("plasma-organizer"));
    setOrganizationDomain(QStringLiteral("dev.quinnjr"));
    setApplicationVersion(QStringLiteral(PROJECT_VERSION_STRING));
}

Application::~Application() = default;

bool Application::init()
{
    qmlRegisterType<FenceFileModel>("dev.quinnjr.organizer", 1, 0, "FenceFileModel");

    m_mgr = new FenceManager(QString(), this);
    m_mgr->load();

    // Create D-Bus interface
    m_dbus = new DBusInterface(this, m_mgr, this);

    // Register on session bus
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService(QStringLiteral("org.kde.plasma.desktoporganizer"))) {
        qWarning() << "Failed to register D-Bus service:" << sessionBus.lastError().message();
    }
    if (!sessionBus.registerObject(QStringLiteral("/org/kde/plasma/desktoporganizer"), m_dbus,
                                    QDBusConnection::ExportScriptableContents)) {
        qWarning() << "Failed to register D-Bus object:" << sessionBus.lastError().message();
    }

    // Connect configuration reload signal
    connect(m_dbus, &DBusInterface::configurationReloaded,
            this, &Application::onConfigurationReloaded);

    for (QScreen *screen : screens()) {
        createWindowForScreen(screen);
    }

    connect(this, &QGuiApplication::screenAdded,
            this, &Application::onScreenAdded);
    connect(this, &QGuiApplication::screenRemoved,
            this, &Application::onScreenRemoved);

    m_tray = new KStatusNotifierItem(this);
    m_tray->setIconByName(QStringLiteral("view-split-left-right"));
    m_tray->setTitle(QStringLiteral("Desktop Organizer"));
    m_tray->setToolTip(QStringLiteral("view-split-left-right"),
                       QStringLiteral("Desktop Organizer"),
                       QStringLiteral("Fences-style desktop organizer"));
    m_tray->setStatus(KStatusNotifierItem::Active);

    auto *menu = m_tray->contextMenu();
    auto *toggleAction = menu->addAction(QStringLiteral("Hide All Fences"));
    connect(toggleAction, &QAction::triggered, this, [this, toggleAction]() {
        const bool currentlyVisible = !m_windows.isEmpty()
                                      && m_windows.front()->isVisible();
        const bool nextVisible = !currentlyVisible;
        toggleAction->setText(nextVisible ? QStringLiteral("Hide All Fences")
                                          : QStringLiteral("Show All Fences"));
        for (auto *win : m_windows) {
            nextVisible ? win->show() : win->hide();
        }
    });

    auto *quitAction = menu->addAction(QStringLiteral("Quit"));
    connect(quitAction, &QAction::triggered, this, &QGuiApplication::quit);

    return true;
}

void Application::createWindowForScreen(QScreen *screen)
{
    auto *win = new LayerShellWindow(screen, m_mgr, this);
    m_windows.append(win);
    qDebug() << "Created window for screen" << screen->name();
}

void Application::onScreenAdded(QScreen *screen)
{
    createWindowForScreen(screen);
}

void Application::onScreenRemoved(QScreen *screen)
{
    for (int i = 0; i < m_windows.size(); ++i) {
        if (m_windows[i]->screen() == screen) {
            delete m_windows.takeAt(i);
            break;
        }
    }
}

void Application::onConfigurationReloaded()
{
    // Update system tray visibility based on settings
    if (m_tray) {
        m_tray->setStatus(OrganizerSettings::instance()->showSystemTray()
                          ? KStatusNotifierItem::Active
                          : KStatusNotifierItem::Passive);
    }
}
