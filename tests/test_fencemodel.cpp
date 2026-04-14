#include <QtTest>
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include "fencemodel.h"
#include "fencemanager.h"

class TestFenceModel : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void modelIsValid() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        // QAbstractItemModelTester runs all Qt model invariant checks
        QAbstractItemModelTester tester(&model,
            QAbstractItemModelTester::FailureReportingMode::Fatal);
        QVERIFY(true);
    }

    void rowCountMatchesFences() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        QCOMPARE(model.rowCount(), 0);

        mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300), QStringLiteral("A"));
        mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300), QStringLiteral("B"));

        QCOMPARE(model.rowCount(), 2);
    }

    void roleDataReturnsCorrectValues() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        Fence f = mgr.createFence(QStringLiteral("DP-2"),
                                   QRect(10, 20, 500, 250),
                                   QStringLiteral("Games"));

        const QModelIndex idx = model.index(0);
        QCOMPARE(model.data(idx, FenceModel::TitleRole).toString(), QStringLiteral("Games"));
        QCOMPARE(model.data(idx, FenceModel::ScreenRole).toString(), QStringLiteral("DP-2"));
        QCOMPARE(model.data(idx, FenceModel::XRole).toInt(), 10);
        QCOMPARE(model.data(idx, FenceModel::YRole).toInt(), 20);
        QCOMPARE(model.data(idx, FenceModel::WidthRole).toInt(), 500);
        QCOMPARE(model.data(idx, FenceModel::HeightRole).toInt(), 250);
        QCOMPARE(model.data(idx, FenceModel::FenceIdRole).toString(), f.id);
    }

    void removeFenceUpdatesModel() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        Fence f = mgr.createFence(QStringLiteral("DP-1"), QRect(0,0,400,300));
        QCOMPARE(model.rowCount(), 1);

        mgr.deleteFence(f.id);
        QCOMPARE(model.rowCount(), 0);
    }
};

QTEST_MAIN(TestFenceModel)
#include "test_fencemodel.moc"
