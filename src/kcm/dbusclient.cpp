#include "dbusclient.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QProcess>

static const QString SERVICE_NAME = QStringLiteral("org.kde.plasma.desktoporganizer");
static const QString OBJECT_PATH = QStringLiteral("/org/kde/plasma/desktoporganizer");
static const QString INTERFACE_NAME = QStringLiteral("org.kde.plasma.DesktopOrganizer");

DBusClient::DBusClient(QObject *parent)
    : QObject(parent)
{
    m_interface = new QDBusInterface(SERVICE_NAME, OBJECT_PATH, INTERFACE_NAME,
                                     QDBusConnection::sessionBus(), this);

    // Poll status every 2 seconds
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &DBusClient::updateStatus);
    m_statusTimer->start(2000);

    // Initial status check
    updateStatus();
}

DBusClient::~DBusClient() = default;

void DBusClient::start()
{
    // Try D-Bus activation first
    auto reply = QDBusConnection::sessionBus().interface()->startService(SERVICE_NAME);
    if (!reply.isValid()) {
        qWarning() << "DBusClient: D-Bus activation failed:" << reply.error().message();
    }

    // If that fails, launch manually
    QTimer::singleShot(1000, this, [this]() {
        if (!m_isRunning) {
            if (!QProcess::startDetached(QStringLiteral("plasma-organizer"), {})) {
                qWarning() << "DBusClient: failed to launch plasma-organizer via QProcess";
            }
        }
    });
}

void DBusClient::stop()
{
    if (m_interface->isValid()) {
        m_interface->call(QStringLiteral("quit"));
    }
}

void DBusClient::restart()
{
    m_statusTimer->stop();
    stop();
    QTimer::singleShot(1500, this, [this]() {
        start();
        m_statusTimer->start(2000);
    });
}

void DBusClient::reloadConfig()
{
    if (m_interface->isValid()) {
        m_interface->call(QStringLiteral("reloadConfiguration"));
    }
}

void DBusClient::updateStatus()
{
    bool wasRunning = m_isRunning;
    m_isRunning = QDBusConnection::sessionBus().interface()->isServiceRegistered(SERVICE_NAME);

    if (m_isRunning != wasRunning) {
        Q_EMIT isRunningChanged();
    }

    if (m_isRunning && m_interface->isValid()) {
        // Update properties from D-Bus
        QVariant countVar = m_interface->property("fenceCount");
        if (countVar.isValid()) {
            int count = countVar.toInt();
            if (count != m_fenceCount) {
                m_fenceCount = count;
                Q_EMIT fenceCountChanged();
            }
        }

        QVariant versionVar = m_interface->property("version");
        if (versionVar.isValid()) {
            QString ver = versionVar.toString();
            if (ver != m_version) {
                m_version = ver;
                Q_EMIT versionChanged();
            }
        }
    } else {
        // Service stopped - reset properties
        if (m_fenceCount != 0) {
            m_fenceCount = 0;
            Q_EMIT fenceCountChanged();
        }
        if (!m_version.isEmpty()) {
            m_version.clear();
            Q_EMIT versionChanged();
        }
    }
}
