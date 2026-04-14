#include "application.h"
#include <QDebug>

Application::Application(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
    setApplicationName(QStringLiteral("plasma-organizer"));
    setOrganizationDomain(QStringLiteral("dev.quinnjr"));
    setApplicationVersion(QStringLiteral("0.1.0"));
}

Application::~Application() = default;

bool Application::init()
{
    qDebug() << "plasma-organizer starting up";
    return true;
}
