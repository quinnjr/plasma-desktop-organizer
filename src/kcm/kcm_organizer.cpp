#include <KPluginFactory>
#include <KQuickConfigModule>
#include <QQmlEngine>
#include "settingsmodel.h"
#include "dbusclient.h"

class KCMOrganizer : public KQuickConfigModule {
    Q_OBJECT
public:
    explicit KCMOrganizer(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        // Register QML types
        qmlRegisterType<SettingsModel>("org.kde.plasma.organizer.kcm", 1, 0, "SettingsModel");
        qmlRegisterSingletonType<DBusClient>("org.kde.plasma.organizer.kcm", 1, 0, "DBusClient",
            [](QQmlEngine *, QJSEngine *) -> QObject * {
                return new DBusClient();
            });

        // Set UI
        setButtons(Apply | Default);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMOrganizer, "kcm_plasma_organizer.json")

#include "kcm_organizer.moc"
