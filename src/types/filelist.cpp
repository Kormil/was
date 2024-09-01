#include "filelist.h"

#include <algorithm>

#include "src/types/dir.h"
#include "src/models/filelistmodel.h"

FileList::FileList(QObject *qboject) :
    QObject(qboject)
{
}

FileList::~FileList() {
}

IdType FileList::id() const {
    return id_;
}

void FileList::setId(IdType id) {
    id_ = id;
}

std::size_t FileList::size() const
{
    return files_.size();
}

FilePtr FileList::getByIndex(int index)
{
    if (index >= files_.size()) {
        return nullptr;
    }

    return files_[index];
}

FilePtr FileList::get(const IdType& id)
{
    auto row = id_to_row_.find(id);
    if (row != id_to_row_.end()) {
        return files_[row->second];
    }

    return nullptr;
}

bool FileList::has(const IdType& id) {
    auto row = id_to_row_.find(id);
    if (row != id_to_row_.end()) {
        return true;
    }

    return false;
}

int FileList::index(const IdType& id) const {
    auto row = id_to_row_.find(id);
    if (row != id_to_row_.end()) {
        return row->second;
    }

    return -1;
}

void FileList::append(const FilePtr &file)
{
    if (file == nullptr) {
        return;
    }

    auto FileIt = id_to_row_.find(file->id());

    if (id_to_row_.end() == FileIt) {
        emit preItemAppended(1);

        int row = files_.size();
        files_.push_back(file);
        id_to_row_[file->id()] = row;

        if (!file->isDir()) {
            ++already_downloaded_files_;
        }

        emit postItemAppended();
    }
}
#include <QDebug>
void FileList::appendList(const FileListPtr &files)
{
    if (!files) {
        return;
    }

    std::vector<FilePtr> temp_files_;

    for (auto& file: files->files_) {
        auto FileIt = id_to_row_.find(file->id());
        if (id_to_row_.end() != FileIt) {
            continue;
        }

        temp_files_.push_back(file);
    }

    if (temp_files_.empty()) {
        return;
    }

    emit preItemAppended(temp_files_.size());

    int row = files_.size();
    auto first_file_it = files_.insert(files_.end(), temp_files_.begin(), temp_files_.end());

    for (; first_file_it != files_.end(); ++first_file_it, ++row) {
        id_to_row_[first_file_it->get()->id()] = row;

        if (!first_file_it->get()->isDir()) {
            ++already_downloaded_files_;
        } else {
            qDebug() << "NO TO DUPA";
        }
    }

    emit postItemAppended();
}

std::vector<IdType> FileList::getAllIds() const {
    std::vector<IdType> ids;

    for (const auto& file: files_) {
        if (file->isDir()) {
            ids.push_back(file->id());
        }
    }

    return ids;
}

QVariant FileList::data(const IdType file_id, int role) {
    const auto& file = get(file_id);

    if (!file) {
        return 0;
    }

    if (role == FileListModel::FileListRole::AllItemsCounterRole) {
        return file->countAllItems();
    }

    return {};
}

void FileList::setData(const IdType file_id, int role, QVariant value) {
    bool visible_data_changed = false;

    const auto& row = id_to_row_.find(file_id);
    if (row == id_to_row_.end()) {
        return;
    }

    const auto &file = files_[row->second];

    if (!file) {
        return ;
    }

    if (role == FileListModel::FileListRole::ItemsCounterRole) {
        file->setItemsCount(value.toInt());
        emit dataChanged(row->second, {FileListModel::FileListRole::AllItemsCounterRole});

        visible_data_changed = true;
    } else if (role == FileListModel::FileListRole::FoldersCounterRole) {
        file->setFoldersCount(value.toInt());
        emit dataChanged(row->second, FileListModel::FileListRole::AllItemsCounterRole);

        visible_data_changed = true;
    }

    if (visible_data_changed) {
        emit dataChanged(row->second, role);
    }
}
#include <QDebug>
unsigned int FileList::countAllItems() const {
    qDebug() << "getAllFilesCounter?";
    if (parent_) {
        qDebug() << "getAllFilesCounter: " << parent_->countAllItems();
        return parent_->countAllItems();
    }

    return 0;
}

unsigned int FileList::countItems() const {
    if (parent_) {
        return parent_->countItems();
    }

    return 0;
}

bool FileList::canFetchMore() const {
    qDebug() << "canFetchMore: already_downloaded_files_: " << already_downloaded_files_;
    qDebug() << "canFetchMore: countItems(): " << countItems();
    qDebug() << "canFetchMore: already_downloaded_files_ < countItems(): " << (already_downloaded_files_ < countItems());
//    if (start_point < already_downloaded_files_) {
//        return false;
//    }

    return already_downloaded_files_ < countItems();
}

void FileList::setParentFile(const FilePtr& parent) {
    parent_ = parent;

    if (parent_) {
        id_ = parent_->id();
    }
}

const FilePtr& FileList::parentFile() {
    return parent_;
}
#include <QDebug>
void FileList::setItemsCount(int count) {
    if (parent_ && count != parent_->countItems()) {
       parent_->setItemsCount(count);
       qDebug() << "setItemsCount: " << count;
       emit countChanged(id());
    }
}
void FileList::setFoldersCount(int count) {
    if (parent_/* && count != parent_->foldersCount()*/) {
       parent_->setFoldersCount(count);
       qDebug() << "setFoldersCount: " << count;
       //emit countChanged();
    }
}
