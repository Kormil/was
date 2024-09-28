#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include <memory>
#include <mutex>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QQuickImageProvider>

#include "src/types/file.h"
#include "src/types/filelist.h"

class FolderListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int folder_id READ folderId WRITE setFolderId NOTIFY folderIdChanged)

public:
    explicit FolderListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void clear();

    FileListPtr files() const;
    void setFiles(const FileListPtr &files);

    FileListPtr folders() const;
    void setFolders(const FileListPtr &folders);

    int folderId() const;
    void setFolderId(int id);

    Q_INVOKABLE int mapToFileListModel(int index) const;

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

signals:
    void filesLoading();
    void foldersLoading();
    void filesLoaded();
    void foldersLoaded();

    void folderIdChanged();

private:
    std::mutex files_mtx_;
    std::mutex folders_mtx_;

    FileListPtr folders_;
    FileListPtr files_;

    int folder_id_;
};

using FolderListModelPtr = std::shared_ptr<FolderListModel>;

#endif // FOLDERLISTMODEL_H
