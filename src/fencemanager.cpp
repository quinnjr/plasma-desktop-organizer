#include "fencemanager.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUuid>
#include <QDebug>

FenceManager *FenceManager::s_instance = nullptr;

FenceManager::FenceManager(const QString &basePath, QObject *parent)
    : QObject(parent)
    , m_basePath(basePath.isEmpty() ? defaultBasePath() : basePath)
{
    if (!s_instance) s_instance = this;
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

QString FenceManager::defaultBasePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
           + QStringLiteral("/dev.quinnjr.desktop-organizer");
}
