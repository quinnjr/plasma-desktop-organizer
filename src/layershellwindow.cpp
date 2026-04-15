#include "layershellwindow.h"
#include "fencemanager.h"
#include "fencemodel.h"
#include "fence.h"

#include <QQmlContext>
#include <QDebug>
#include <QRegion>
#include <LayerShellQt/Window>

LayerShellWindow::LayerShellWindow(QScreen *screen,
                                    FenceManager *mgr,
                                    QObject *parent)
    : QObject(parent)
    , m_screen(screen)
    , m_mgr(mgr)
    , m_model(new FenceModel(mgr, this))
    , m_view(std::make_unique<QQuickView>())
{
    m_view->setScreen(screen);
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->resize(screen->geometry().size());
    m_view->setColor(Qt::transparent);

    // Expose C++ objects to QML
    m_view->rootContext()->setContextProperty(
        QStringLiteral("fenceModel"), m_model);
    m_view->rootContext()->setContextProperty(
        QStringLiteral("fenceManager"), m_mgr);
    m_view->rootContext()->setContextProperty(
        QStringLiteral("screenName"),
        screen->name());

    m_view->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    setupLayerShell();

    connect(m_mgr, &FenceManager::fenceAdded,   this, &LayerShellWindow::updateInputRegion);
    connect(m_mgr, &FenceManager::fenceRemoved, this, &LayerShellWindow::updateInputRegion);
    connect(m_mgr, &FenceManager::fenceUpdated, this, &LayerShellWindow::updateInputRegion);
    updateInputRegion();
}

LayerShellWindow::~LayerShellWindow() = default;

void LayerShellWindow::setupLayerShell()
{
    // show() creates the native Wayland surface; LayerShellQt::Window::get()
    // must be called after the surface exists.
    m_view->show();

    auto *layerShell = LayerShellQt::Window::get(m_view.get());
    if (!layerShell) {
        qWarning() << "LayerShellWindow: compositor does not support"
                      " zwlr_layer_shell_v1; overlay will not appear";
        return;
    }
    layerShell->setLayer(LayerShellQt::Window::LayerBottom);
    layerShell->setAnchors(LayerShellQt::Window::Anchors(
        LayerShellQt::Window::AnchorTop
        | LayerShellQt::Window::AnchorBottom
        | LayerShellQt::Window::AnchorLeft
        | LayerShellQt::Window::AnchorRight));
    layerShell->setExclusiveZone(-1);
    layerShell->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityOnDemand);
}

void LayerShellWindow::updateInputRegion()
{
    QRegion region;
    const QString myScreen = m_screen->name();

    for (const Fence &f : m_mgr->fences()) {
        if (f.screen != myScreen) continue;
        const int height = f.rolledUp ? 28 : f.height;
        region += QRect(f.x, f.y, f.width, height);
    }

    m_view->setMask(region);
}

void LayerShellWindow::show()
{
    m_view->show();
}

void LayerShellWindow::hide()
{
    m_view->hide();
}
