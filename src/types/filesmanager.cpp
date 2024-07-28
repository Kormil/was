#include "filesmanager.h"

#include <algorithm>

#include "src/settings.h"
#include "src/models/filelistmodel.h"
#include "src/connection/requests/countfoldersrequest.h"
#include "src/connection/requests/countitemsrequest.h"
#include "src/connection/requests/getfoldersrequest.h"
#include "src/connection/requests/getitemsrequest.h"

FilesManager::FilesManager(const ConnectionPtr& connection) :
    connection_(connection) {
    file_lists_iterator_ = file_lists_.begin();
}

void FilesManager::clear() {
    file_lists_.clear();
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

    auto space = Settings::instance()->apiSpace();

    for (const auto& file: *files) {
        if (file->isDir()) {
            auto count_items_request = std::make_shared<CountItemsRequest>(file->id(), space, handler);
            auto count_items_serial = connection_->addRequest(count_items_request);
            connection_->runRequest(count_items_serial);

            auto count_folders_request = std::make_shared<CountFoldersRequest>(file->id(), space, handler);
            auto count_folders_serial = connection_->addRequest(count_folders_request);
            connection_->runRequest(count_folders_serial);
        }
    }

    // for main dir
    auto count_items_request = std::make_shared<CountItemsRequest>(files->id(), space, handler);
    auto count_items_serial = connection_->addRequest(count_items_request);
    connection_->runRequest(count_items_serial);

    auto count_folders_request = std::make_shared<CountFoldersRequest>(files->id(), space, handler);
    auto count_folders_serial = connection_->addRequest(count_folders_request);
    connection_->runRequest(count_folders_serial);
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

    auto space = Settings::instance()->apiSpace();
    auto request = std::make_shared<GetFoldersRequest>(id, space, [this, id, start_point, handler](const FileListPtr &files) {
        append(files);

        handler(file_lists_iterator_->second);
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

void FilesManager::fetchMoreItems(IdType id, unsigned int start_point, std::function<void(const FileListPtr &files)> handler) {
    auto space = Settings::instance()->apiSpace();

    auto request = std::make_shared<GetItemsRequest>(id, start_point, space, [this, handler](const FileListPtr &files) {
        append(files);
        handler(file_lists_iterator_->second);
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
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
        if (file_lists_iterator_->first == id) {
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
