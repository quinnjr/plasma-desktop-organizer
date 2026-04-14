#include "application.h"
#include <QDebug>

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
    qDebug() << "plasma-organizer starting up";
    return true;
}
