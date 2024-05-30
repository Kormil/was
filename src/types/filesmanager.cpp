#include "filesmanager.h"

#include <algorithm>

#include "src/models/filelistmodel.h"

namespace {
IdType InvalidId = -1;
IdType RootId = 0;
}

FilesManager::FilesManager(const ConnectionPtr& connection) :
    connection_(connection) {
    file_lists_iterator_ = file_lists_.begin();
}

std::size_t FilesManager::size() const {
    return file_lists_.size();
}

void FilesManager::pop() {
    std::lock_guard lock{file_list_iterator_mutex_};

    if (file_lists_iterator_ == file_lists_.begin()) {
        return ;
    }

    --file_lists_iterator_;
}

void FilesManager::getItemsCounter(const FileListPtr &files) {
    auto handler = [files](int subdir_id, int count) {
        qDebug() << files->id() << " : " << count;

        if (!count) {
            return;
        }

        // for a parent
        if (subdir_id == files->id() && files->parentFile()) {
            files->parentFile()->setCount(count);
            return;
        }


        // for rest of files
        auto current_count = files->data(subdir_id, FileListModel::FileListRole::ItemCounterRole).toInt();
        files->setData(subdir_id, FileListModel::FileListRole::ItemCounterRole, count + current_count);
    };

    for (const auto& file: *files) {
        if (file->isDir()) {
            connection_->countOfDirectoryItems(file->id(), handler);
            connection_->countOfDirectory(file->id(), handler);
        }
    }

    // for main dir
    connection_->countOfDirectoryItems(files->id(), handler);
    connection_->countOfDirectory(files->id(), handler);
}

void FilesManager::get(IdType id, unsigned int start_point, std::function<void(const FileListPtr &files)> handler) {
    std::lock_guard lock{file_list_iterator_mutex_};
    if (file_lists_iterator_ != file_lists_.end() && id == file_lists_iterator_->first) {
        auto& file_list = file_lists_iterator_->second;

        qDebug() << start_point << " >= " << file_list->size();
        if (file_list->canFetchMore() && start_point >= file_list->size()) {
            return fetchMoreItems(id, start_point, handler);
        } else {
            return handler(file_list);
        }
    }

//    const auto next_id = getNextId();
//    if (next_id != InvalidId) {
//        ++file_lists_iterator_;
//        return handler(top());
//    }

    if (file_lists_iterator_ != file_lists_.end()) {
        file_lists_iterator_ = file_lists_.erase((file_lists_iterator_ + 1), file_lists_.end());
        --file_lists_iterator_;
    }

    connection_->contentOfDirectory(id, [this, id, start_point, handler](const FileListPtr &files) {
        append(files);

        handler(file_lists_iterator_->second);
        //fetchMoreItems(id, start_point, handler);
    });

}

void FilesManager::fetchMoreItems(IdType id, unsigned int start_point, std::function<void(const FileListPtr &files)> handler) {
    connection_->contentOfDirectoryItems(id, start_point, [this, handler](const FileListPtr &files) {
        append(files);
        handler(file_lists_iterator_->second);
    });
}

FilePtr FilesManager::getFile(IdType file_id) {
    if (!file_id) {
        return nullptr;
    }

    for (auto& file_list: file_lists_) {
        for (const auto &file: *file_list.second) {
            if (file->id() == file_id) {
                return file;
            }
        }
    }

    return nullptr;
}

void FilesManager::append(const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};
    const auto id = files->id();

    // got something already
    if (file_lists_iterator_ != file_lists_.end()) {
        // Przy pobiernaiu zdjeć to files->id() jest 0 ponieważ nie ma parenta
        if (file_lists_iterator_->first == id) {

            qDebug() << "DUPA22232: " << file_lists_iterator_->second->id();
            file_lists_iterator_->second->appendList(files);
            return;
        }
    }

    // new items
    const auto &parent = getFile(id);
    files->setParentFile(parent);

    file_lists_.push_back({id, files});

    // set iterator on last element :(
    file_lists_iterator_ = file_lists_.end();
    --file_lists_iterator_;
}
