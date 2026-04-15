#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QScopeGuard>
#include <QStandardPaths>
#include "fencemanager.h"

class TestFenceManager : public QObject {
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        // Redirect all QStandardPaths locations to ~/.qttest/ to prevent
        // any test from accidentally touching real user data directories.
        QStandardPaths::setTestModeEnabled(true);
    }

    void cleanupTestCase() {
        QStandardPaths::setTestModeEnabled(false);
    }

    // ── existing tests (unchanged) ──────────────────────────────────────────

    void createFence_addsToList() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());

        Fence f = mgr.createFence(QStringLiteral("DP-1"),
                                   QRect(100, 200, 400, 300),
                                   QStringLiteral("Games"));

        QCOMPARE(mgr.fences().size(), 1);
        QCOMPARE(mgr.fences().first().title, QStringLiteral("Games"));
        QCOMPARE(mgr.fences().first().screen, QStringLiteral("DP-1"));
        QCOMPARE(mgr.fences().first().x, 100);
        QCOMPARE(mgr.fences().first().width, 400);
        QVERIFY(!f.id.isEmpty());
    }

    void createFence_createsDirectory() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());

        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300));
        QString dir = mgr.fenceDirectory(f.id);

        QVERIFY(QDir(dir).exists());
    }

    void deleteFence_removesFromList() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());

        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300));
        QCOMPARE(mgr.fences().size(), 1);

        QSignalSpy spy(&mgr, &FenceManager::fenceRemoved);
        mgr.deleteFence(f.id);
        QCOMPARE(mgr.fences().size(), 0);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toInt(), 0);
    }

    void jsonRoundTrip() {
        QTemporaryDir tmp;
        {
            FenceManager mgr(tmp.path());
            mgr.createFence(QStringLiteral("DP-2"),
                            QRect(50, 60, 500, 250),
                            QStringLiteral("Work"));
            mgr.save();
        }
        {
            FenceManager mgr2(tmp.path());
            mgr2.load();
            QCOMPARE(mgr2.fences().size(), 1);
            const Fence &f = mgr2.fences().first();
            QCOMPARE(f.title, QStringLiteral("Work"));
            QCOMPARE(f.screen, QStringLiteral("DP-2"));
            QCOMPARE(f.x, 50);
            QCOMPARE(f.y, 60);
            QCOMPARE(f.width, 500);
            QCOMPARE(f.height, 250);
            QCOMPARE(f.rolledUp, false);
            QCOMPARE(f.iconSize, 64);
        }
    }

    void updateFence_updatesInMemory() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300));

        f.title = QStringLiteral("Renamed");
        f.x = 999;
        mgr.updateFence(f);

        const Fence &updated = mgr.fences().first();
        QCOMPARE(updated.title, QStringLiteral("Renamed"));
        QCOMPARE(updated.x, 999);
    }

    // ── new tests ───────────────────────────────────────────────────────────

    void createFence_emitsFenceAdded() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());

        QSignalSpy spy(&mgr, &FenceManager::fenceAdded);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toInt(), 0);
    }

    void createFenceOnScreen_returnsIdAndPersists() {
        QTemporaryDir tmp;
        {
            FenceManager mgr(tmp.path());
            QString id = mgr.createFenceOnScreen(
                QStringLiteral("DP-1"), 10, 20, 300, 200,
                QStringLiteral("OnScreen"));

            QVERIFY(!id.isEmpty());
            QCOMPARE(mgr.fences().size(), 1);
            QCOMPARE(mgr.fences().first().x, 10);
        }
        // createFenceOnScreen calls save() — verify it persisted
        {
            FenceManager mgr2(tmp.path());
            mgr2.load();
            QCOMPARE(mgr2.fences().size(), 1);
            QCOMPARE(mgr2.fences().first().title, QStringLiteral("OnScreen"));
        }
    }

    void instance_returnsConstructedManager() {
        // Each test method runs sequentially; the previous test's FenceManager
        // destructor has already cleared s_instance before this runs.
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        QCOMPARE(FenceManager::instance(), &mgr);
    }

    void renameFence_updatesTitleAndEmitsSignal() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.renameFence(f.id, QStringLiteral("New Name"));

        QCOMPARE(mgr.fences().first().title, QStringLiteral("New Name"));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toInt(), 0);
    }

    void renameFence_unknownId_isNoOp() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300),
                        QStringLiteral("Original"));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.renameFence(QStringLiteral("no-such-id"), QStringLiteral("X"));

        QCOMPARE(spy.count(), 0);
        QCOMPARE(mgr.fences().first().title, QStringLiteral("Original"));
    }

    void moveFence_updatesPositionAndEmitsSignal() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.moveFence(f.id, 100, 200);

        QCOMPARE(mgr.fences().first().x, 100);
        QCOMPARE(mgr.fences().first().y, 200);
        QCOMPARE(spy.count(), 1);
    }

    void moveFence_unknownId_isNoOp() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.createFence(QStringLiteral("DP-1"), QRect(5, 6, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.moveFence(QStringLiteral("no-such-id"), 99, 99);

        QCOMPARE(spy.count(), 0);
        QCOMPARE(mgr.fences().first().x, 5);
    }

    void resizeFence_updatesAllDimensionsAndEmitsSignal() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.resizeFence(f.id, 50, 60, 800, 600);

        const Fence &r = mgr.fences().first();
        QCOMPARE(r.x, 50);
        QCOMPARE(r.y, 60);
        QCOMPARE(r.width, 800);
        QCOMPARE(r.height, 600);
        QCOMPARE(spy.count(), 1);
    }

    void resizeFence_unknownId_isNoOp() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.resizeFence(QStringLiteral("no-such-id"), 1, 2, 3, 4);

        QCOMPARE(spy.count(), 0);
        QCOMPARE(mgr.fences().first().width, 400);
    }

    void setRolledUp_updatesFieldAndEmitsSignal() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));
        QVERIFY(!mgr.fences().first().rolledUp);

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.setRolledUp(f.id, true);

        QVERIFY(mgr.fences().first().rolledUp);
        QCOMPARE(spy.count(), 1);
    }

    void setRolledUp_unknownId_isNoOp() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        QSignalSpy spy(&mgr, &FenceManager::fenceUpdated);
        mgr.setRolledUp(QStringLiteral("no-such-id"), true);

        QCOMPARE(spy.count(), 0);
        QVERIFY(!mgr.fences().first().rolledUp);
    }

    void load_withMalformedJson_doesNotCrash() {
        QTemporaryDir tmp;
        {
            QFile f(tmp.path() + QStringLiteral("/fences.json"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("not json at all }{");
        }
        FenceManager mgr(tmp.path());
        mgr.load();
        QCOMPARE(mgr.fences().size(), 0);
    }

    void load_withNonArrayJson_doesNotCrash() {
        QTemporaryDir tmp;
        {
            QFile f(tmp.path() + QStringLiteral("/fences.json"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("{\"key\": \"value\"}");
        }
        FenceManager mgr(tmp.path());
        mgr.load();
        QCOMPARE(mgr.fences().size(), 0);
    }

    void load_skipsEntriesWithEmptyId() {
        QTemporaryDir tmp;
        {
            QFile f(tmp.path() + QStringLiteral("/fences.json"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write(R"([
                {"id":"","title":"Bad","screen":"DP-1","x":0,"y":0,
                 "width":400,"height":300,"rolledUp":false,"iconSize":64},
                {"id":"valid-uuid","title":"Good","screen":"DP-1","x":0,"y":0,
                 "width":400,"height":300,"rolledUp":false,"iconSize":64}
            ])");
        }
        FenceManager mgr(tmp.path());
        mgr.load();
        QCOMPARE(mgr.fences().size(), 1);
        QCOMPARE(mgr.fences().first().title, QStringLiteral("Good"));
    }

    void save_withReadOnlyConfigFile_doesNotCrash() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));
        mgr.save(); // create the file first

        const QString configPath = tmp.path() + QStringLiteral("/fences.json");
        // Guarantee permission restoration even if the test aborts mid-way.
        auto restorePerms = qScopeGuard([&] {
            QFile::setPermissions(configPath,
                QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        });
        QFile::setPermissions(configPath,
            QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);

        mgr.save(); // must not crash — just warns
    }

    void moveUrlsToFence_withMissingDirectory_doesNotCrash() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.moveUrlsToFence({QStringLiteral("file:///tmp/any.txt")},
                            QStringLiteral("fake-uuid-no-dir"));
    }

    void moveUrlsToFence_withAllInvalidUrls_doesNotCrash() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));
        mgr.moveUrlsToFence({QStringLiteral(""), QStringLiteral(":::bad:::")}, f.id);
    }

    void moveUrlsToDesktop_withAllInvalidUrls_doesNotCrash() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        mgr.moveUrlsToDesktop({QStringLiteral(""), QStringLiteral(":::bad:::")});
    }
};

QTEST_MAIN(TestFenceManager)
#include "test_fencemanager.moc"
