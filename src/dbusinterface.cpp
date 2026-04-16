#include "dbusinterface.h"
#include "application.h"
#include "fencemanager.h"
#include "layershellwindow.h"
#include "organizersettings.h"

DBusInterface::DBusInterface(Application *app, FenceManager *mgr, QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_mgr(mgr)
{
}

DBusInterface::~DBusInterface() = default;

void DBusInterface::showAllFences()
{
    for (auto *window : m_app->windows()) {
        window->show();
    }
}

void DBusInterface::hideAllFences()
{
    for (auto *window : m_app->windows()) {
        window->hide();
    }
}

void DBusInterface::reloadConfiguration()
{
    OrganizerSettings::instance()->reload();
    Q_EMIT configurationReloaded();
}

void DBusInterface::quit()
{
    m_app->quit();
}

int DBusInterface::fenceCount() const
{
    return m_mgr->fences().size();
}

QString DBusInterface::version() const
{
    return QStringLiteral(PROJECT_VERSION_STRING);
}
