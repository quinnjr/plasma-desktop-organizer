#include "application.h"
#include "fencefilemodel.h"
#include "fencemanager.h"
#include "layershellwindow.h"

#include <QScreen>
#include <QDebug>
#include <QtQml/qqml.h>

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

    for (QScreen *screen : screens()) {
        createWindowForScreen(screen);
    }

    connect(this, &QGuiApplication::screenAdded,
            this, &Application::onScreenAdded);
    connect(this, &QGuiApplication::screenRemoved,
            this, &Application::onScreenRemoved);

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
