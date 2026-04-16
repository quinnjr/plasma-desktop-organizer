#pragma once
#include <QObject>

class Application;
class FenceManager;

class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.DesktopOrganizer")
public:
    explicit DBusInterface(Application *app, FenceManager *mgr, QObject *parent = nullptr);
    ~DBusInterface() override;

public Q_SLOTS:
    Q_SCRIPTABLE void showAllFences();
    Q_SCRIPTABLE void hideAllFences();
    Q_SCRIPTABLE void reloadConfiguration();
    Q_SCRIPTABLE void quit();

    Q_SCRIPTABLE bool isRunning() const { return true; }
    Q_SCRIPTABLE int fenceCount() const;
    Q_SCRIPTABLE QString version() const;

Q_SIGNALS:
    Q_SCRIPTABLE void configurationReloaded();

private:
    Application *m_app;
    FenceManager *m_mgr;
};
