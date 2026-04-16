#include <QtTest>
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include <QStandardPaths>
#include "fencemodel.h"
#include "fencemanager.h"
#include "organizersettings.h"

class TestFenceModel : public QObject {
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        m_settings = new OrganizerSettings(this);
    }

    void cleanupTestCase() {
        delete m_settings;
        m_settings = nullptr;
        QStandardPaths::setTestModeEnabled(false);
    }

private:
    OrganizerSettings *m_settings = nullptr;

    // ── existing tests (unchanged) ──────────────────────────────────────────

    void modelIsValid() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        QAbstractItemModelTester tester(&model,
            QAbstractItemModelTester::FailureReportingMode::Fatal);
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
        QCOMPARE(model.data(idx, FenceModel::TitleRole).toString(),  QStringLiteral("Games"));
        QCOMPARE(model.data(idx, FenceModel::ScreenRole).toString(), QStringLiteral("DP-2"));
        QCOMPARE(model.data(idx, FenceModel::XRole).toInt(),         10);
        QCOMPARE(model.data(idx, FenceModel::YRole).toInt(),         20);
        QCOMPARE(model.data(idx, FenceModel::WidthRole).toInt(),     500);
        QCOMPARE(model.data(idx, FenceModel::HeightRole).toInt(),    250);
        QCOMPARE(model.data(idx, FenceModel::FenceIdRole).toString(), f.id);
        QCOMPARE(model.data(idx, FenceModel::RolledUpRole).toBool(), false);
        QCOMPARE(model.data(idx, FenceModel::IconSizeRole).toInt(),  64);
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

    // ── new tests ───────────────────────────────────────────────────────────

    void data_withInvalidIndex_returnsEmpty() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        // Default (invalid) index
        QVERIFY(!model.data(QModelIndex(), FenceModel::TitleRole).isValid());
        // Out-of-range index (no fences added yet)
        QVERIFY(!model.data(model.index(0), FenceModel::TitleRole).isValid());
    }

    void data_withUnknownRole_returnsEmpty() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        // Qt::DecorationRole is not implemented — must return {}
        QVERIFY(!model.data(model.index(0), Qt::DecorationRole).isValid());
    }

    void data_withDisplayRole_returnsTitle() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300),
                        QStringLiteral("MyFence"));

        QCOMPARE(model.data(model.index(0), Qt::DisplayRole).toString(),
                 QStringLiteral("MyFence"));
    }

    void flags_withValidIndex_isEnabledAndEditable() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        const Qt::ItemFlags f = model.flags(model.index(0));
        QVERIFY(f & Qt::ItemIsEnabled);
        QVERIFY(f & Qt::ItemIsEditable);
    }

    void flags_withInvalidIndex_returnsNoFlags() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
    }

    void setData_updatesTitle() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300),
                        QStringLiteral("Original"));

        QVERIFY(model.setData(model.index(0),
                              QStringLiteral("Renamed"),
                              FenceModel::TitleRole));
        QCOMPARE(mgr.fences().first().title, QStringLiteral("Renamed"));
    }

    void setData_updatesGeometry() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        const QModelIndex idx = model.index(0);
        QVERIFY(model.setData(idx, 100, FenceModel::XRole));
        QVERIFY(model.setData(idx, 200, FenceModel::YRole));
        QVERIFY(model.setData(idx, 800, FenceModel::WidthRole));
        QVERIFY(model.setData(idx, 600, FenceModel::HeightRole));

        const Fence &f = mgr.fences().first();
        QCOMPARE(f.x,      100);
        QCOMPARE(f.y,      200);
        QCOMPARE(f.width,  800);
        QCOMPARE(f.height, 600);
    }

    void setData_updatesRolledUpAndIconSize() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        const QModelIndex idx = model.index(0);
        QVERIFY(model.setData(idx, true, FenceModel::RolledUpRole));
        QVERIFY(model.setData(idx, 32,   FenceModel::IconSizeRole));

        QVERIFY(mgr.fences().first().rolledUp);
        QCOMPARE(mgr.fences().first().iconSize, 32);
    }

    void setData_withInvalidIndex_returnsFalse() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);

        QVERIFY(!model.setData(QModelIndex(),
                               QStringLiteral("x"),
                               FenceModel::TitleRole));
    }

    void setData_withReadOnlyRole_returnsFalse() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300));

        const QModelIndex idx = model.index(0);
        // FenceIdRole and ScreenRole are read-only in setData
        QVERIFY(!model.setData(idx, QStringLiteral("x"), FenceModel::FenceIdRole));
        QVERIFY(!model.setData(idx, QStringLiteral("x"), FenceModel::ScreenRole));
    }

    void fenceUpdated_triggersDataChanged() {
        QTemporaryDir tmp;
        FenceManager mgr(tmp.path());
        FenceModel model(&mgr);
        mgr.createFence(QStringLiteral("DP-1"), QRect(0, 0, 400, 300),
                        QStringLiteral("Original"));

        QSignalSpy spy(&model, &QAbstractItemModel::dataChanged);

        Fence f = mgr.fences().first();
        f.title = QStringLiteral("Updated");
        mgr.updateFence(f);

        QCOMPARE(spy.count(), 1);
        // Verify the index that changed is row 0
        const QModelIndex changed = spy.first().first().value<QModelIndex>();
        QCOMPARE(changed.row(), 0);
    }
};

QTEST_MAIN(TestFenceModel)
#include "test_fencemodel.moc"
