#include "fencemodel.h"
#include "fencemanager.h"

FenceModel::FenceModel(FenceManager *mgr, QObject *parent)
    : QAbstractListModel(parent)
    , m_mgr(mgr)
{
    connect(mgr, &FenceManager::fenceAdded, this, [this](int index) {
        beginInsertRows(QModelIndex(), index, index);
        endInsertRows();
    });
    connect(mgr, &FenceManager::fenceRemoved, this, [this](int index) {
        beginRemoveRows(QModelIndex(), index, index);
        endRemoveRows();
    });
    connect(mgr, &FenceManager::fenceUpdated, this, [this](int index) {
        const QModelIndex mi = this->index(index);
        Q_EMIT dataChanged(mi, mi);
    });
}

int FenceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_mgr->fences().size();
}

QVariant FenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_mgr->fences().size())
        return {};

    const Fence &f = m_mgr->fences().at(index.row());
    switch (role) {
    case FenceIdRole:  return f.id;
    case TitleRole:    return f.title;
    case ScreenRole:   return f.screen;
    case XRole:        return f.x;
    case YRole:        return f.y;
    case WidthRole:    return f.width;
    case HeightRole:   return f.height;
    case RolledUpRole: return f.rolledUp;
    case IconSizeRole: return f.iconSize;
    case Qt::DisplayRole: return f.title;
    default:           return {};
    }
}

bool FenceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_mgr->fences().size())
        return false;

    Fence f = m_mgr->fences().at(index.row());
    switch (role) {
    case TitleRole:    f.title    = value.toString(); break;
    case XRole:        f.x        = value.toInt();    break;
    case YRole:        f.y        = value.toInt();    break;
    case WidthRole:    f.width    = value.toInt();    break;
    case HeightRole:   f.height   = value.toInt();    break;
    case RolledUpRole: f.rolledUp = value.toBool();   break;
    case IconSizeRole: f.iconSize = value.toInt();    break;
    default: return false;
    }
    m_mgr->updateFence(f);
    return true;
}

Qt::ItemFlags FenceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QHash<int, QByteArray> FenceModel::roleNames() const
{
    return {
        {FenceIdRole,  "fenceId"},
        {TitleRole,    "title"},
        {ScreenRole,   "screen"},
        {XRole,        "fenceX"},
        {YRole,        "fenceY"},
        {WidthRole,    "fenceWidth"},
        {HeightRole,   "fenceHeight"},
        {RolledUpRole, "rolledUp"},
        {IconSizeRole, "iconSize"},
    };
}
