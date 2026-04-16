#include <QtTest>
#include <QStandardPaths>
#include "organizersettings.h"

class TestOrganizerSettings : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
    }

    void cleanupTestCase() {
        QStandardPaths::setTestModeEnabled(false);
    }

    void cleanup() {
        // Reset singleton between tests
        OrganizerSettings::destroy();
    }

    void defaultValues_returnHardcodedDefaults() {
        OrganizerSettings *settings = OrganizerSettings::instance();
        QCOMPARE(settings->defaultIconSize(), 64);
        QCOMPARE(settings->showSystemTray(), true);
        QCOMPARE(settings->fenceOpacity(), 85);
    }

    void setAndGet_persistsValues() {
        OrganizerSettings *settings = OrganizerSettings::instance();
        settings->setDefaultIconSize(48);
        settings->sync();

        // Reload from disk
        OrganizerSettings::destroy();
        settings = OrganizerSettings::instance();
        QCOMPARE(settings->defaultIconSize(), 48);
    }
};

QTEST_MAIN(TestOrganizerSettings)
#include "test_organizersettings.moc"
