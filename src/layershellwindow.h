#pragma once
#include <QObject>
#include <QQuickView>
#include <QScreen>

class FenceManager;
class FenceModel;

class LayerShellWindow : public QObject {
    Q_OBJECT
public:
    explicit LayerShellWindow(QScreen *screen,
                               FenceManager *mgr,
                               QObject *parent = nullptr);
    ~LayerShellWindow() override;

    QScreen *screen() const { return m_screen; }
    void show();
    void hide();

private:
    void setupLayerShell();

    QScreen *m_screen;
    FenceManager *m_mgr;
    FenceModel *m_model;
    QQuickView *m_view;
};
