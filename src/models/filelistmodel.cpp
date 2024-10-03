#include "filelistmodel.h"

#include "src/connection/controller.h"

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent),
      items_(nullptr)
{
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || items_ == nullptr)
        return 0;

    return items_->size();
}

void FileListModel::clear() {
    setList(nullptr);
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || items_ == nullptr)
        return QVariant();

    const FilePtr& file = items_->getByIndex(index.row());
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
    case FileListRole::AllItemsCounterRole: {
        return QVariant(file->countAllItems());
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
    names[FileListRole::AllItemsCounterRole] = "item_counter";
    names[FileListRole::PhotoSourceRole] = "photo_source";

    return names;
}

void FileListModel::appendList(const FileListPtr &files) {
    items_->appendList(files);
}

void FileListModel::setList(const FileListPtr &files)
{
    if (files == items_) {
        return;
    }

    beginResetModel();

    if (items_) {
        items_->disconnect(this);
    }

    items_ = files;

    if (items_) {
        connect(items_.get(), &FileList::preItemAppended, this, [this](int size) {
            const int index = items_->size();
            beginInsertRows(QModelIndex(), index, index + size - 1);
        });

        connect(items_.get(), &FileList::postItemAppended, this, [this]() {
            endInsertRows();
        });

        connect(items_.get(), &FileList::dataChanged, this, [this](int row, int role) {
            auto index = createIndex(row, 0);
            emit dataChanged(index, index, {role});
        });

        connect(items_.get(), &FileList::countChanged, this, [this](int id) {
            if (canFetchMore(QModelIndex())) {
                fetchMore(QModelIndex());
            }
        });
    }

    endResetModel();
}

bool FileListModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid() || !items_) {
        return false;
    }

    const bool can_fetch_more = items_->canFetchMore();
    if (!can_fetch_more) {
        emit filesLoaded();
    }

    return can_fetch_more;
}

void FileListModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || !items_) {
        return;
    }

    auto& controller = Controller::instance();
    controller.getItemsInFolder(items_->id());
}

int FileListModel::folderId()  const {
    return folder_id_;
}

void FileListModel::setFolderId(int id) {
    folder_id_ = id;

    auto& controller = Controller::instance();
    const auto files = controller.getItemsInFolder(folder_id_);

    setList(files);
}
