#include "layershellwindow.h"
#include "fencemanager.h"
#include "fencemodel.h"

#include <QQmlContext>
#include <QDebug>
#include <LayerShellQt/Window>

LayerShellWindow::LayerShellWindow(QScreen *screen,
                                    FenceManager *mgr,
                                    QObject *parent)
    : QObject(parent)
    , m_screen(screen)
    , m_mgr(mgr)
    , m_model(new FenceModel(mgr, this))
    , m_view(new QQuickView)
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
}

LayerShellWindow::~LayerShellWindow()
{
    delete m_view;
}

void LayerShellWindow::setupLayerShell()
{
    // show() creates the native Wayland surface; LayerShellQt::Window::get()
    // must be called after the surface exists.
    m_view->show();

    auto *layerShell = LayerShellQt::Window::get(m_view);
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

void LayerShellWindow::show()
{
    m_view->show();
}

void LayerShellWindow::hide()
{
    m_view->hide();
}
