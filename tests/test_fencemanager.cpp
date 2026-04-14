#include <QtTest>
#include <QTemporaryDir>
#include "fencemanager.h"

class TestFenceManager : public QObject {
    Q_OBJECT

private Q_SLOTS:
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

        mgr.deleteFence(f.id);
        QCOMPARE(mgr.fences().size(), 0);
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
        }
    }

    void updateFence_persistsChanges() {
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
};

QTEST_MAIN(TestFenceManager)
#include "test_fencemanager.moc"
