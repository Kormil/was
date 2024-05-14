#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QQuickImageProvider>

#include "src/types/file.h"
#include "src/types/filelist.h"

struct PhotoResolution {
    unsigned int width;
    unsigned int height;
};

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum FileListRole
    {
        NameRole = Qt::UserRole + 1,
        IdRole,
        ParentIdRole,
        ResolutionRole,
        IsDirRole,
        CacheKeyRole,
        CreateTimeRole,
        ItemCounterRole,
        PhotoSourceRole
    };

    explicit FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setList(FileListPtr files);

signals:
    void filesLoaded();

private:
    FileListPtr files_;
};

using FileListModelPtr = std::shared_ptr<FileListModel>;

#endif // FILELISTMODEL_H
