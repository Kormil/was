#include "filelistmodel.h"

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent),
      files_(nullptr)
{

}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || files_ == nullptr)
        return 0;

    return files_->size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || files_ == nullptr)
        return QVariant();

    const FilePtr file = files_->getByIndex(index.row());
    if (!file) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case FileListRole::NameRole: {
        return QVariant(file->name());
    }
    case FileListRole::IdRole: {
        return QVariant(file->id());
    }
    case FileListRole::ParentIdRole: {
        return QVariant(file->parentId());
    }
    case FileListRole::IsDirRole: {
        return QVariant(file->isDir());
    }
    case FileListRole::CacheKeyRole: {
        return QVariant(file->cacheKey());
    }
    case FileListRole::CreateTimeRole: {
        return QVariant(file->createTime());
    }
    case FileListRole::ItemCounterRole: {
        return QVariant(file->count());
    }
    case FileListRole::PhotoSourceRole: {
        return QVariant(file->photoSource());
    }
    }

    return QVariant();
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::DisplayRole] = "display";

    names[FileListRole::NameRole] = "name";
    names[FileListRole::IdRole] = "id";
    names[FileListRole::ParentIdRole] = "parent_id";
    names[FileListRole::IsDirRole] = "is_dir";
    names[FileListRole::CacheKeyRole] = "cache_key";
    names[FileListRole::CreateTimeRole] = "create_time";
    names[FileListRole::ItemCounterRole] = "item_counter";
    names[FileListRole::PhotoSourceRole] = "photo_source";

    return names;
}

void FileListModel::setList(FileListPtr files)
{
    if (files == files_) {
        return;
    }

    beginResetModel();

    if (files_) {
        files_->disconnect(this);
    }

    files_ = std::move(files);

    if (files_) {
        connect(files_.get(), &FileList::preItemAppended, this, [this](int size) {
            const int index = files_->size();
            beginInsertRows(QModelIndex(), index, index + size - 1);
        });

        connect(files_.get(), &FileList::postItemAppended, this, [this]() {
            endInsertRows();
        });

        connect(files_.get(), &FileList::dataChanged, this, [this](int row, int role) {
            auto index = createIndex(row, 0);
            emit dataChanged(index, index, {role});
        });
    }

    endResetModel();
    emit filesLoaded();
}
