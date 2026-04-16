#include "fencemanager.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUuid>
#include <QDebug>
#include <QtGlobal>
#include <KIO/CopyJob>
#include <KJob>
#include "organizersettings.h"

FenceManager *FenceManager::s_instance = nullptr;

FenceManager::FenceManager(const QString &basePath, QObject *parent)
    : QObject(parent)
    , m_basePath(basePath)
{
    if (!s_instance) s_instance = this;

    // Determine actual base path
    if (m_basePath.isEmpty()) {
        m_basePath = OrganizerSettings::instance()->dataDirectory();
    }

    m_configPath = m_basePath + QStringLiteral("/fences.json");
    QDir().mkpath(m_basePath);
}

FenceManager::~FenceManager()
{
    if (s_instance == this) s_instance = nullptr;
}

FenceManager *FenceManager::instance()
{
    Q_ASSERT_X(s_instance, "FenceManager::instance",
               "FenceManager must be constructed before calling instance()");
    return s_instance;
}

const QVector<Fence> &FenceManager::fences() const
{
    return m_fences;
}

Fence FenceManager::createFence(const QString &screen,
                                 const QRect &geometry,
                                 const QString &title)
{
    Fence f;
    f.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    f.title = title;
    f.iconSize = OrganizerSettings::instance()->defaultIconSize();
    f.screen = screen;
    f.x = geometry.x();
    f.y = geometry.y();
    f.width = geometry.width();
    f.height = geometry.height();

    QDir().mkpath(fenceDirectory(f.id));

    const int index = m_fences.size();
    m_fences.append(f);
    Q_EMIT fenceAdded(index);
    return f;
}

QString FenceManager::createFenceOnScreen(const QString &screen,
                                           int x, int y, int w, int h,
                                           const QString &title)
{
    Fence f = createFence(screen, QRect(x, y, qMax(1, w), qMax(1, h)), title);
    save();
    return f.id;
}

void FenceManager::deleteFence(const QString &id)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == id) {
            Q_EMIT fenceRemoved(i);
            m_fences.removeAt(i);
            return;
        }
    }
}

void FenceManager::updateFence(const Fence &fence)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == fence.id) {
            m_fences[i] = fence;
            Q_EMIT fenceUpdated(i);
            return;
        }
    }
}

void FenceManager::renameFence(const QString &id, const QString &title)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == id) {
            m_fences[i].title = title;
            Q_EMIT fenceUpdated(i);
            save();
            return;
        }
    }
}

void FenceManager::moveFence(const QString &id, int x, int y)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == id) {
            m_fences[i].x = x;
            m_fences[i].y = y;
            Q_EMIT fenceUpdated(i);
            save();
            return;
        }
    }
}

void FenceManager::resizeFence(const QString &id, int x, int y, int width, int height)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == id) {
            m_fences[i].x = x;
            m_fences[i].y = y;
            m_fences[i].width = width;
            m_fences[i].height = height;
            Q_EMIT fenceUpdated(i);
            save();
            return;
        }
    }
}

void FenceManager::setRolledUp(const QString &id, bool rolledUp)
{
    for (int i = 0; i < m_fences.size(); ++i) {
        if (m_fences[i].id == id) {
            m_fences[i].rolledUp = rolledUp;
            Q_EMIT fenceUpdated(i);
            save();
            return;
        }
    }
}

QString FenceManager::fenceDirectory(const QString &fenceId) const
{
    return m_basePath + QStringLiteral("/fences/") + fenceId;
}

void FenceManager::load()
{
    QFile f(m_configPath);
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    m_fences.clear();
    for (const QJsonValue &val : doc.array()) {
        const QJsonObject obj = val.toObject();
        Fence fence;
        fence.id       = obj.value(QLatin1String("id")).toString();
        fence.title    = obj.value(QLatin1String("title")).toString();
        fence.screen   = obj.value(QLatin1String("screen")).toString();
        fence.x        = obj.value(QLatin1String("x")).toInt();
        fence.y        = obj.value(QLatin1String("y")).toInt();
        fence.width    = obj.value(QLatin1String("width")).toInt(400);
        fence.height   = obj.value(QLatin1String("height")).toInt(300);
        fence.rolledUp = obj.value(QLatin1String("rolledUp")).toBool(false);
        fence.iconSize = obj.value(QLatin1String("iconSize")).toInt(64);
        if (!fence.id.isEmpty()) {
            m_fences.append(fence);
        }
    }
}

void FenceManager::save()
{
    QJsonArray arr;
    for (const Fence &f : m_fences) {
        QJsonObject obj;
        obj.insert(QLatin1String("id"),       f.id);
        obj.insert(QLatin1String("title"),    f.title);
        obj.insert(QLatin1String("screen"),   f.screen);
        obj.insert(QLatin1String("x"),        f.x);
        obj.insert(QLatin1String("y"),        f.y);
        obj.insert(QLatin1String("width"),    f.width);
        obj.insert(QLatin1String("height"),   f.height);
        obj.insert(QLatin1String("rolledUp"), f.rolledUp);
        obj.insert(QLatin1String("iconSize"), f.iconSize);
        arr.append(obj);
    }

    QFile file(m_configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(arr).toJson());
    } else {
        qWarning() << "FenceManager: could not write" << m_configPath;
    }
}

void FenceManager::moveUrlsToFence(const QStringList &urls, const QString &fenceId)
{
    const QString destDir = fenceDirectory(fenceId);
    if (!QDir().exists(destDir)) {
        qWarning() << "FenceManager::moveUrlsToFence: directory does not exist:" << destDir;
        return;
    }

    QList<QUrl> srcUrls;
    srcUrls.reserve(urls.size());
    for (const QString &s : urls) {
        const QUrl u(s);
        if (u.isValid() && !u.isEmpty())
            srcUrls.append(u);
        else
            qWarning() << "FenceManager::moveUrlsToFence: skipping invalid URL:" << s;
    }
    if (srcUrls.isEmpty()) return;

    // LCOV_EXCL_START - KIO async ops require a live KIO scheduler; covered by integration tests
    auto *job = KIO::move(srcUrls, QUrl::fromLocalFile(destDir));
    // KIO jobs auto-delete; do not store or delete this pointer
    connect(job, &KJob::result, this, [](KJob *job) {
        if (job->error()) {
            qWarning() << "KIO move failed:" << job->errorString();
        }
    });
    job->start();
    // LCOV_EXCL_STOP
}

void FenceManager::moveUrlsToDesktop(const QStringList &urls)
{
    const QString desktopPath = QStandardPaths::writableLocation(
        QStandardPaths::DesktopLocation);
    if (desktopPath.isEmpty()) {
        qWarning() << "FenceManager::moveUrlsToDesktop: DesktopLocation unresolvable";
        return;
    }

    QList<QUrl> srcUrls;
    srcUrls.reserve(urls.size());
    for (const QString &s : urls) {
        const QUrl u(s);
        if (u.isValid() && !u.isEmpty())
            srcUrls.append(u);
        else
            qWarning() << "FenceManager::moveUrlsToDesktop: skipping invalid URL:" << s;
    }
    if (srcUrls.isEmpty()) return;

    // LCOV_EXCL_START - KIO async ops require a live KIO scheduler; covered by integration tests
    auto *job = KIO::move(srcUrls, QUrl::fromLocalFile(desktopPath));
    // KIO jobs auto-delete; do not store or delete this pointer
    connect(job, &KJob::result, this, [](KJob *job) {
        if (job->error()) {
            qWarning() << "KIO move failed:" << job->errorString();
        }
    });
    job->start();
    // LCOV_EXCL_STOP
}

QString FenceManager::defaultBasePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
           + QStringLiteral("/dev.quinnjr.desktop-organizer");
}

void FenceManager::onConfigurationReloaded()
{
    // Check if data directory changed (requires restart)
    const QString newDataDir = OrganizerSettings::instance()->dataDirectory();
    if (newDataDir != m_basePath) {
        qWarning() << "Data directory changed to" << newDataDir
                   << "- restart required for this change to take effect";
    }

    // Future fence creations will use updated default icon size
}
