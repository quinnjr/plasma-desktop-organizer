#pragma once
#include <QGuiApplication>
#include <QList>
#include <KStatusNotifierItem>

class FenceManager;
class LayerShellWindow;

class Application : public QGuiApplication {
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application() override;

    bool init();

    const QList<LayerShellWindow *> &windows() const { return m_windows; }

private Q_SLOTS:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);

private:
    void createWindowForScreen(QScreen *screen);

    FenceManager *m_mgr = nullptr;
    QList<LayerShellWindow *> m_windows;
    KStatusNotifierItem *m_tray = nullptr;
};
