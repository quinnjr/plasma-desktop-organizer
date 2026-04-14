#include "fencefilemodel.h"

#include <QDir>
#include <QFileInfo>
#include <KDesktopFile>

FenceFileModel::FenceFileModel(const QString &directory, QObject *parent)
    : QAbstractListModel(parent)
    , m_directory(directory)
    , m_watch(new KDirWatch(this))
{
    if (!directory.isEmpty()) {
        m_watch->addDir(directory, KDirWatch::WatchFiles);
    }
    connect(m_watch, &KDirWatch::dirty,   this, &FenceFileModel::onDirty);
    connect(m_watch, &KDirWatch::created, this, &FenceFileModel::onCreated);
    connect(m_watch, &KDirWatch::deleted, this, &FenceFileModel::onDeleted);
    if (!directory.isEmpty()) {
        refresh();
    }
}

FenceFileModel::~FenceFileModel() = default;

void FenceFileModel::setDirectory(const QString &directory)
{
    if (m_directory == directory) return;

    if (!m_directory.isEmpty()) {
        m_watch->removeDir(m_directory);
    }
    m_directory = directory;
    if (!m_directory.isEmpty()) {
        m_watch->addDir(m_directory, KDirWatch::WatchFiles);
        refresh();
    }
}

int FenceFileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant FenceFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) return {};
    const KFileItem &item = m_items.at(index.row());

    switch (role) {
    case FileNameRole:
        return item.name();
    case FileUrlRole:
        return item.url().toString();
    case IsDirRole:
        return item.isDir();
    case DisplayNameRole: {
        if (item.name().endsWith(QLatin1String(".desktop"))) {
            auto it = m_desktopCache.constFind(item.localPath());
            if (it != m_desktopCache.constEnd() && !it->first.isEmpty())
                return it->first;
        }
        return item.name();
    }
    case IconNameRole: {
        if (item.name().endsWith(QLatin1String(".desktop"))) {
            auto it = m_desktopCache.constFind(item.localPath());
            if (it != m_desktopCache.constEnd() && !it->second.isEmpty())
                return it->second;
        }
        return item.iconName();
    }
    case Qt::DisplayRole:
        return data(index, DisplayNameRole);
    default:
        return {};
    }
}

QHash<int, QByteArray> FenceFileModel::roleNames() const
{
    return {
        {FileNameRole,    "fileName"},
        {FileUrlRole,     "fileUrl"},
        {IconNameRole,    "iconName"},
        {DisplayNameRole, "displayName"},
        {IsDirRole,       "isDir"},
    };
}

void FenceFileModel::onDirty(const QString &path)
{
    if (path == m_directory) {
        refresh();
    } else {
        int i = findByPath(path);
        if (i >= 0) {
            m_items[i].refresh();
            if (m_items[i].name().endsWith(QLatin1String(".desktop"))) {
                KDesktopFile df(path);
                m_desktopCache.insert(path, {df.readName(), df.readIcon()});
            } else {
                m_desktopCache.remove(path);
            }
            const QModelIndex mi = index(i);
            Q_EMIT dataChanged(mi, mi);
        }
    }
}

void FenceFileModel::onCreated(const QString &path)
{
    if (QFileInfo(path).dir().absolutePath() != QDir(m_directory).absolutePath())
        return;
    if (findByPath(path) >= 0) return; // already have it

    KFileItem item(QUrl::fromLocalFile(path));
    const int newIndex = m_items.size();
    beginInsertRows(QModelIndex(), newIndex, newIndex);
    m_items.append(item);
    endInsertRows();
}

void FenceFileModel::onDeleted(const QString &path)
{
    int i = findByPath(path);
    if (i < 0) return;
    beginRemoveRows(QModelIndex(), i, i);
    m_items.removeAt(i);
    endRemoveRows();
}

void FenceFileModel::refresh()
{
    beginResetModel();
    m_items.clear();
    QDir dir(m_directory);
    for (const QFileInfo &fi : dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        m_items.append(KFileItem(QUrl::fromLocalFile(fi.absoluteFilePath())));
    }
    endResetModel();

    // Populate .desktop metadata cache
    m_desktopCache.clear();
    for (const KFileItem &item : m_items) {
        if (item.name().endsWith(QLatin1String(".desktop"))) {
            KDesktopFile df(item.localPath());
            m_desktopCache.insert(item.localPath(),
                                  {df.readName(), df.readIcon()});
        }
    }
}

int FenceFileModel::findByPath(const QString &path) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].localPath() == path) return i;
    }
    return -1;
}
