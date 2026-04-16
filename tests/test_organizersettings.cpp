#include <QtTest>
#include <QStandardPaths>
#include <QTemporaryDir>
#include "organizersettings.h"

class TestOrganizerSettings : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        // Ensure no prior config file exists
        QDir configDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
        configDir.remove(QStringLiteral("plasma-desktop-organizerrc"));
    }

    void cleanupTestCase() {
        QStandardPaths::setTestModeEnabled(false);
    }

    void cleanup() {
        // Delete singleton if it exists to ensure clean state between tests
        if (OrganizerSettings::s_instance) {
            delete OrganizerSettings::s_instance;
            OrganizerSettings::s_instance = nullptr;
        }
    }

    void defaultValues_returnHardcodedDefaults() {
        OrganizerSettings settings; // Local instance
        QCOMPARE(settings.defaultIconSize(), 64);
        QCOMPARE(settings.showSystemTray(), true);
        QCOMPARE(settings.fenceOpacity(), 85);
    }

    void setAndGet_persistsValues() {
        {
            OrganizerSettings settings; // First instance
            settings.setDefaultIconSize(48);
            settings.sync();
        } // Destruction clears s_instance

        {
            OrganizerSettings settings; // New instance, reads persisted value
            QCOMPARE(settings.defaultIconSize(), 48);
        }
    }

    void fenceOpacity_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setFenceOpacity(-10);
        QCOMPARE(settings.fenceOpacity(), 0);

        settings.setFenceOpacity(150);
        QCOMPARE(settings.fenceOpacity(), 100);

        settings.setFenceOpacity(50);
        QCOMPARE(settings.fenceOpacity(), 50);
    }

    void defaultIconSize_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setDefaultIconSize(8);
        QCOMPARE(settings.defaultIconSize(), 16);

        settings.setDefaultIconSize(512);
        QCOMPARE(settings.defaultIconSize(), 256);

        settings.setDefaultIconSize(96);
        QCOMPARE(settings.defaultIconSize(), 96);
    }

    void titleBarHeight_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setTitleBarHeight(5);
        QCOMPARE(settings.titleBarHeight(), 16);

        settings.setTitleBarHeight(200);
        QCOMPARE(settings.titleBarHeight(), 100);

        settings.setTitleBarHeight(48);
        QCOMPARE(settings.titleBarHeight(), 48);
    }

    void gridSize_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setGridSize(1);
        QCOMPARE(settings.gridSize(), 5);

        settings.setGridSize(150);
        QCOMPARE(settings.gridSize(), 100);

        settings.setGridSize(32);
        QCOMPARE(settings.gridSize(), 32);
    }

    void minFenceWidth_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setMinFenceWidth(10);
        QCOMPARE(settings.minFenceWidth(), 50);

        settings.setMinFenceWidth(1000);
        QCOMPARE(settings.minFenceWidth(), 500);

        settings.setMinFenceWidth(200);
        QCOMPARE(settings.minFenceWidth(), 200);
    }

    void minFenceHeight_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setMinFenceHeight(5);
        QCOMPARE(settings.minFenceHeight(), 40);

        settings.setMinFenceHeight(500);
        QCOMPARE(settings.minFenceHeight(), 400);

        settings.setMinFenceHeight(120);
        QCOMPARE(settings.minFenceHeight(), 120);
    }

    void clickThroughDelay_clamps_outOfRange() {
        OrganizerSettings settings;
        settings.setClickThroughDelay(-100);
        QCOMPARE(settings.clickThroughDelay(), 0);

        settings.setClickThroughDelay(10000);
        QCOMPARE(settings.clickThroughDelay(), 5000);

        settings.setClickThroughDelay(250);
        QCOMPARE(settings.clickThroughDelay(), 250);
    }
};

QTEST_MAIN(TestOrganizerSettings)
#include "test_organizersettings.moc"
