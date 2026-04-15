#include <QtTest>
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "fencefilemodel.h"

class TestFenceFileModel : public QObject {
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
    }

    void cleanupTestCase() {
        QStandardPaths::setTestModeEnabled(false);
    }

    // ── existing tests (unchanged) ──────────────────────────────────────────

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
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("hello");
        f.close();

        QVERIFY(spy.wait(2000));
        QCOMPARE(model.rowCount(), 1);
    }

    void filenameRoleReturnsName() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/readme.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());

        QCOMPARE(model.rowCount(), 1);
        const QModelIndex idx = model.index(0);
        QCOMPARE(model.data(idx, FenceFileModel::FileNameRole).toString(),
                 QStringLiteral("readme.txt"));
    }

    // ── new tests ───────────────────────────────────────────────────────────

    void rowCount_withValidParent_returnsZero() {
        // FenceFileModel is a flat list; any child query must return 0.
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/a.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0)), 0);
    }

    void data_withInvalidIndex_returnsEmpty() {
        QTemporaryDir tmp;
        FenceFileModel model(tmp.path());

        QVERIFY(!model.data(QModelIndex(), FenceFileModel::FileNameRole).isValid());
        QVERIFY(!model.data(model.index(0), FenceFileModel::FileNameRole).isValid());
    }

    void data_fileUrlRole_returnsAbsoluteFileUrl() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/hello.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);

        const QString url =
            model.data(model.index(0), FenceFileModel::FileUrlRole).toString();
        QVERIFY(url.startsWith(QStringLiteral("file://")));
        QVERIFY(url.endsWith(QStringLiteral("hello.txt")));
    }

    void data_isDirRole_falseForRegularFile() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/test.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());
        QVERIFY(!model.data(model.index(0), FenceFileModel::IsDirRole).toBool());
    }

    void data_isDirRole_trueForSubdirectory() {
        QTemporaryDir tmp;
        QDir(tmp.path()).mkdir(QStringLiteral("subdir"));

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);
        QVERIFY(model.data(model.index(0), FenceFileModel::IsDirRole).toBool());
    }

    void data_displayNameRole_nonDesktop_returnsFilename() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/notes.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());
        QCOMPARE(
            model.data(model.index(0), FenceFileModel::DisplayNameRole).toString(),
            QStringLiteral("notes.txt"));
    }

    void data_displayRole_returnsDisplayName() {
        QTemporaryDir tmp;
        QFile f(tmp.path() + QStringLiteral("/notes.txt"));
        QVERIFY(f.open(QIODevice::WriteOnly)); f.write("x"); f.close();

        FenceFileModel model(tmp.path());
        QCOMPARE(
            model.data(model.index(0), Qt::DisplayRole).toString(),
            QStringLiteral("notes.txt"));
    }

    void data_displayNameRole_desktopFile_returnsDesktopName() {
        QTemporaryDir tmp;
        {
            QFile f(tmp.path() + QStringLiteral("/myapp.desktop"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("[Desktop Entry]\nType=Application\nName=My Application\nIcon=utilities-terminal\n");
        }

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(
            model.data(model.index(0), FenceFileModel::DisplayNameRole).toString(),
            QStringLiteral("My Application"));
    }

    void data_iconNameRole_desktopFile_returnsDesktopIcon() {
        QTemporaryDir tmp;
        {
            QFile f(tmp.path() + QStringLiteral("/myapp.desktop"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("[Desktop Entry]\nType=Application\nName=My Application\nIcon=utilities-terminal\n");
        }

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(
            model.data(model.index(0), FenceFileModel::IconNameRole).toString(),
            QStringLiteral("utilities-terminal"));
    }

    void setDirectory_switchesToNewDirectory() {
        QTemporaryDir tmp1, tmp2;
        {
            QFile f(tmp2.path() + QStringLiteral("/file.txt"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("x");
        }

        FenceFileModel model(tmp1.path());
        QCOMPARE(model.rowCount(), 0);

        model.setDirectory(tmp2.path());
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.directory(), tmp2.path());
    }

    void setDirectory_withSameDirectory_doesNotEmitSignal() {
        QTemporaryDir tmp;
        FenceFileModel model(tmp.path());

        QSignalSpy spy(&model, &FenceFileModel::directoryChanged);
        model.setDirectory(tmp.path());
        QCOMPARE(spy.count(), 0);
    }

    void detectsDeletedFile() {
        QTemporaryDir tmp;
        const QString filePath = tmp.path() + QStringLiteral("/todelete.txt");
        {
            QFile f(filePath);
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("x");
        }

        FenceFileModel model(tmp.path());
        QCOMPARE(model.rowCount(), 1);

        QSignalSpy spy(&model, &FenceFileModel::rowsRemoved);
        QFile::remove(filePath);

        QVERIFY(spy.wait(2000));
        QCOMPARE(model.rowCount(), 0);
    }
};

QTEST_MAIN(TestFenceFileModel)
#include "test_fencefilemodel.moc"
