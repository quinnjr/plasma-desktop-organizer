#include <QtTest>
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "fencefilemodel.h"

class TestFenceFileModelModifications : public QObject {
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
    }

    void cleanupTestCase() {
        QStandardPaths::setTestModeEnabled(false);
    }

    void detectsModifiedFile() {
        QTemporaryDir tmp;
        const QString filePath = tmp.path() + QStringLiteral("/test.txt");
        {
            QFile f(filePath);
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("original content");
        }

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);

        QSignalSpy spy(&model, &FenceFileModel::dataChanged);

        // Modify the file
        {
            QFile f(filePath);
            QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Append));
            f.write(" - modified");
        }

        // KDirWatch is async — give it up to 2 seconds
        QVERIFY(spy.wait(2000));
        QCOMPARE(model.rowCount(), 1); // still 1 file
    }

    void detectsModifiedDesktopFile() {
        QTemporaryDir tmp;
        const QString filePath = tmp.path() + QStringLiteral("/app.desktop");
        {
            QFile f(filePath);
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("[Desktop Entry]\nType=Application\nName=Original\nIcon=icon1\n");
        }

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0), FenceFileModel::DisplayNameRole).toString(),
                 QStringLiteral("Original"));

        QSignalSpy spy(&model, &FenceFileModel::dataChanged);

        // Modify the .desktop file
        {
            QFile f(filePath);
            QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
            f.write("[Desktop Entry]\nType=Application\nName=Modified\nIcon=icon2\n");
        }

        QVERIFY(spy.wait(2000));
        // Cache should have been updated
        QCOMPARE(model.data(model.index(0), FenceFileModel::DisplayNameRole).toString(),
                 QStringLiteral("Modified"));
        QCOMPARE(model.data(model.index(0), FenceFileModel::IconNameRole).toString(),
                 QStringLiteral("icon2"));
    }

    void defaultConstructor_createsEmptyModel() {
        FenceFileModel model;
        QCOMPARE(model.rowCount(), 0);
        QVERIFY(model.directory().isEmpty());
    }
};

QTEST_MAIN(TestFenceFileModelModifications)
#include "test_fencefilemodel_modifications.moc"
