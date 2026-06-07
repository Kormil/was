#include "folderlistmodel.h"

#include "src/types/ifileprovider.h"
#include "src/models/filelistmodel.h"

FolderListModel::FolderListModel(QObject *parent) :
    QAbstractListModel(parent),
    folders_(nullptr),
    files_(nullptr)
{
}

int FolderListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    int size = 0;

    if (files_) {
        size += files_->size();
    }

    if (folders_) {
        size += folders_->size();
    }

    return size;
}

void FolderListModel::clear() {
    setFiles(nullptr);
    setFolders(nullptr);
}

QVariant FolderListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    FilePtr file;

    if (index.row() < folders_->size()) {
        if (folders_ != nullptr) {
            file = folders_->getByIndex(index.row());
        }
    } else if (files_ != nullptr) {
        file = files_->getByIndex(index.row() - folders_->size());
    }

    if (!file) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case FileListModel::FileListRole::NameRole: {
        return QVariant(file->name());
    }
    case FileListModel::FileListRole::IdRole: {
        return QVariant(file->id());
    }
    case FileListModel::FileListRole::ParentIdRole: {
        return QVariant(file->parentId());
    }
    case FileListModel::FileListRole::IsDirRole: {
        return QVariant(file->isDir());
    }
    case FileListModel::FileListRole::CacheKeyRole: {
        return QVariant(file->cacheKey());
    }
    case FileListModel::FileListRole::CreateTimeRole: {
        return QVariant(file->createTime());
    }
    case FileListModel::FileListRole::AllItemsCounterRole: {
        return QVariant(file->countAllItems());
    }
    case FileListModel::FileListRole::PhotoSourceRole: {
        return QVariant(file->photoSource());
    }
    }

    return QVariant();
}

QHash<int, QByteArray> FolderListModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::DisplayRole] = "display";

    names[FileListModel::FileListRole::NameRole] = "name";
    names[FileListModel::FileListRole::IdRole] = "id";
    names[FileListModel::FileListRole::ParentIdRole] = "parent_id";
    names[FileListModel::FileListRole::IsDirRole] = "is_dir";
    names[FileListModel::FileListRole::CacheKeyRole] = "cache_key";
    names[FileListModel::FileListRole::CreateTimeRole] = "create_time";
    names[FileListModel::FileListRole::AllItemsCounterRole] = "item_counter";
    names[FileListModel::FileListRole::PhotoSourceRole] = "photo_source";

    return names;
}

void FolderListModel::setFiles(const FileListPtr &files)
{
    if (files == files_) {
        return;
    }

    beginResetModel();


    if (files_) {
        files_->disconnect(this);
    }

    files_ = files;

    if (files_) {
        connect(files_.get(), &FileList::preItemAppended, this, [this](int size) {
            auto folders_size = 0;
            if (folders_) {
                folders_size = folders_->size();
            }

            const int index = files_->size() + folders_size;
            beginInsertRows(QModelIndex(), index, index + size - 1);
        });

        connect(files_.get(), &FileList::postItemAppended, this, [this]() {
            endInsertRows();
        });

        connect(files_.get(), &FileList::loaded, this, [this]() {
            emit filesLoaded();
        });

        connect(files_.get(), &FileList::dataChanged, this, [this](int row, int role) {
            auto folders_size = 0;
            if (folders_) {
                folders_size = folders_->size();
            }
            auto index = createIndex(row + folders_size, 0);
            emit dataChanged(index, index, {role});
        });

        connect(files_.get(), &FileList::countChanged, this, [this](int id) {
            if (canFetchMore(QModelIndex())) {
                fetchMore(QModelIndex());
            }
        });
    }


    endResetModel();
}

void FolderListModel::setFolders(const FileListPtr &files)
{
    if (files == folders_) {
        return;
    }

    beginResetModel();


    if (folders_) {
        folders_->disconnect(this);
    }

    folders_ = files;

    if (folders_) {
        connect(folders_.get(), &FileList::preItemAppended, this, [this](int size) {
            const int index = folders_->size();
            beginInsertRows(QModelIndex(), index, index + size - 1);
        });

        connect(folders_.get(), &FileList::postItemAppended, this, [this]() {
            endInsertRows();
        });

        connect(folders_.get(), &FileList::loaded, this, [this]() {
            emit foldersLoaded();
        });

        connect(folders_.get(), &FileList::dataChanged, this, [this](int row, int role) {
            auto index = createIndex(row, 0);
            emit dataChanged(index, index, {role});
        });
    }

    endResetModel();
    emit foldersLoaded();
}

bool FolderListModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid() || !files_) {
        return false;
    }

    return files_->canFetchMore();
}

void FolderListModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || !files_ || provider_ == nullptr) {
        return;
    }

    emit filesLoading();

    provider_->getItemsInFolder(files_->id());
}

FileListPtr FolderListModel::folders() const {
    return folders_;
}

FileListPtr FolderListModel::files() const {
    return files_;
}

int FolderListModel::folderId()  const {
    return folder_id_;
}

void FolderListModel::setFolderId(int id) {
    folder_id_ = id;

    if (provider_ == nullptr) {
        is_provider_ready_ = false;
        return;
    }

    loadFolder();
}

QObject *FolderListModel::dataSource() const
{
    return dynamic_cast<QObject*>(provider_);
}

void FolderListModel::setDataSource(QObject *source)
{
    auto* provider = dynamic_cast<IFileProvider*>(source);
    if (provider == provider_) {
        return;    if (!is_provider_ready_ && provider_ != nullptr) {
            is_provider_ready_ = true;
            loadFolder();
        }
    }

    provider_ = provider;
    emit dataSourceChanged();

    if (!is_provider_ready_ && provider_ != nullptr) {
        is_provider_ready_ = true;
        loadFolder();
    }
}

int FolderListModel::mapToFileListModel(int index) const {
    if (folders_ == nullptr) {
        return 0;
    }

    return index - folders_->size();
}

void FolderListModel::loadFolder() {
    emit filesLoading();
    emit foldersLoading();

    const auto folders = provider_->getFolders(folder_id_);
    const auto files = provider_->getItemsInFolder(folder_id_);

    setFolders(folders);
    setFiles(files);
}
