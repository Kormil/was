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
    Q_PROPERTY(int folder_id READ folderId WRITE setFolderId NOTIFY folderIdChanged)

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
        AllItemsCounterRole,
        ItemsCounterRole,
        FoldersCounterRole,
        PhotoSourceRole
    };

    explicit FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void appendList(const FileListPtr &files);
    void setList(const FileListPtr &files);

    int folderId() const;
    void setFolderId(int id);

    void clear();

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

signals:
    void filesLoaded() const;
    void folderIdChanged();

private:
    FileListPtr items_;
    int folder_id_;
};

using FileListModelPtr = std::shared_ptr<FileListModel>;

#endif // FILELISTMODEL_H
