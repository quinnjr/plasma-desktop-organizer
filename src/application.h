#pragma once
#include <QGuiApplication>
#include <QList>

class FenceManager;
class LayerShellWindow;

class Application : public QGuiApplication {
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application() override;

    bool init();

private Q_SLOTS:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);

private:
    void createWindowForScreen(QScreen *screen);

    FenceManager *m_mgr = nullptr;
    QList<LayerShellWindow *> m_windows;
};
