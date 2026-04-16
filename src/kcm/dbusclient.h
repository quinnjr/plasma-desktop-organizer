#pragma once
#include <QObject>
#include <QDBusInterface>
#include <QTimer>

class DBusClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(int fenceCount READ fenceCount NOTIFY fenceCountChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)

public:
    explicit DBusClient(QObject *parent = nullptr);
    ~DBusClient() override;

    bool isRunning() const { return m_isRunning; }
    int fenceCount() const { return m_fenceCount; }
    QString version() const { return m_version; }

public Q_SLOTS:
    void start();
    void stop();
    void restart();
    void reloadConfig();

Q_SIGNALS:
    void isRunningChanged();
    void fenceCountChanged();
    void versionChanged();

private Q_SLOTS:
    void updateStatus();

private:
    QDBusInterface *m_interface = nullptr;
    QTimer *m_statusTimer = nullptr;
    bool m_isRunning = false;
    int m_fenceCount = 0;
    QString m_version;
};
