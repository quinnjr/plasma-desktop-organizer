#pragma once
#include <QAbstractListModel>
#include <QHash>
#include <QPair>
#include <QUrl>
#include <KDirWatch>
#include <KFileItem>

class FenceFileModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        FileUrlRole,
        IconNameRole,
        DisplayNameRole,
        IsDirRole,
    };

    explicit FenceFileModel(const QString &directory, QObject *parent = nullptr);
    ~FenceFileModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString directory() const { return m_directory; }
    void setDirectory(const QString &directory);

private Q_SLOTS:
    void onDirty(const QString &path);
    void onCreated(const QString &path);
    void onDeleted(const QString &path);

private:
    void refresh();
    int findByPath(const QString &path) const;

    QString m_directory;
    KDirWatch *m_watch;
    QList<KFileItem> m_items;

    // Cache for .desktop file Name= and Icon= fields; keyed by local path.
    // Populated in refresh(), updated in onDirty() for individual file changes.
    mutable QHash<QString, QPair<QString, QString>> m_desktopCache;
};
