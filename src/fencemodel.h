#pragma once
#include <QAbstractListModel>
#include "fence.h"

class FenceManager;

class FenceModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        FenceIdRole = Qt::UserRole + 1,
        TitleRole,
        ScreenRole,
        XRole,
        YRole,
        WidthRole,
        HeightRole,
        RolledUpRole,
        IconSizeRole,
    };

    explicit FenceModel(FenceManager *mgr, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    FenceManager *m_mgr;
};
