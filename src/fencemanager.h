#pragma once
#include <QObject>
#include <QRect>
#include <QVector>
#include "fence.h"

class FenceManager : public QObject {
    Q_OBJECT
public:
    explicit FenceManager(const QString &basePath = QString(),
                          QObject *parent = nullptr);
    ~FenceManager() override;

    static FenceManager *instance();

    const QVector<Fence> &fences() const;
    Fence createFence(const QString &screen,
                      const QRect &geometry,
                      const QString &title = QStringLiteral("New Fence"));
    void deleteFence(const QString &id);
    void updateFence(const Fence &fence);

    Q_INVOKABLE QString fenceDirectory(const QString &fenceId) const;

    Q_INVOKABLE QString createFenceOnScreen(const QString &screen,
                                             int x, int y, int w, int h,
                                             const QString &title = QStringLiteral("New Fence"));

    Q_INVOKABLE void renameFence(const QString &id, const QString &title);
    Q_INVOKABLE void moveFence(const QString &id, int x, int y);
    Q_INVOKABLE void resizeFence(const QString &id, int x, int y, int width, int height);
    Q_INVOKABLE void setRolledUp(const QString &id, bool rolledUp);

    void load();
    void save();

Q_SIGNALS:
    void fenceAdded(int index);
    void fenceRemoved(int index);
    void fenceUpdated(int index);

private:
    static FenceManager *s_instance;
    QString m_basePath;
    QString m_configPath;
    QVector<Fence> m_fences;

    QString defaultBasePath() const;
};
