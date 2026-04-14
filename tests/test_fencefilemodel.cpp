#include <QtTest>
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include <QFile>
#include "fencefilemodel.h"

class TestFenceFileModel : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void emptyDirectoryHasZeroRows() {
        QTemporaryDir tmp;
        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 0);
    }

    void modelIsValid() {
        QTemporaryDir tmp;
        FenceFileModel model(tmp.path());
        QAbstractItemModelTester tester(&model,
            QAbstractItemModelTester::FailureReportingMode::Fatal);
    }

    void detectsNewFile() {
        QTemporaryDir tmp;
        FenceFileModel model(tmp.path());

        QSignalSpy spy(&model, &FenceFileModel::rowsInserted);

        QFile f(tmp.path() + QStringLiteral("/test.txt"));
        f.open(QIODevice::WriteOnly);
        f.write("hello");
        f.close();

        // KDirWatch is async — give it up to 2 seconds
        QVERIFY(spy.wait(2000));
        QCOMPARE(model.rowCount(), 1);
    }

    void filenameRoleReturnsName() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/readme.txt"));
        f.open(QIODevice::WriteOnly); f.write("x"); f.close();

        FenceFileModel model(tmp.path());

        QCOMPARE(model.rowCount(), 1);
        const QModelIndex idx = model.index(0);
        QCOMPARE(model.data(idx, FenceFileModel::FileNameRole).toString(),
                 QStringLiteral("readme.txt"));
    }
};

QTEST_MAIN(TestFenceFileModel)
#include "test_fencefilemodel.moc"
