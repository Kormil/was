#include "filesmanager.h"

#include <algorithm>

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

FileListPtr FilesManager::top() {
    std::lock_guard lock{file_list_iterator_mutex_};

    if (file_lists_iterator_ == file_lists_.begin()) {
        return nullptr;
    }

    return file_lists_iterator_->second;
}

void FilesManager::pop() {
    std::lock_guard lock{file_list_iterator_mutex_};

    if (file_lists_iterator_ == file_lists_.begin()) {
        return ;
    }

    --file_lists_iterator_;
}

void FilesManager::get(IdType id, std::function<void(const FileListPtr &files)> handler) {
    const auto next_id = getNextId();

    std::lock_guard lock{file_list_iterator_mutex_};

    if (next_id != InvalidId) {
        ++file_lists_iterator_;
        return handler(top());
    }

    file_lists_.erase(file_lists_iterator_, file_lists_.end());

    if (id == RootId) {
        connection_->contentOfPhotoDirectory([this, handler](const FileListPtr &files) {
            init(files);
            handler(files);
        });
    } else {
        connection_->contentOfDirectory(id, [this, id, handler](const FileListPtr &files) {
            append(id, files);
            handler(file_lists_iterator_->second);
        });

        connection_->contentOfDirectoryItems(id, [this, id, handler](const FileListPtr &files) {
            append(id, files);
            handler(file_lists_iterator_->second);
        });
    }
}

IdType FilesManager::getNextId() {
    if (file_lists_.size() == 0) {
        return InvalidId;
    }

    const auto next_ = file_lists_iterator_ + 1;

    if (next_ == file_lists_.end()) {
        return InvalidId;
    }

    return next_->first;
}

void FilesManager::init(const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};

    // new items
    file_lists_.push_back({RootId, files});

    // set iterator on last element :(
    file_lists_iterator_ = file_lists_.end();
    --file_lists_iterator_;
}

void FilesManager::append(IdType id, const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};

    // got something already
    if (file_lists_iterator_->first == id) {
        file_lists_iterator_->second->appendList(files);
        return;
    }

    // new items
    file_lists_.push_back({id, files});

    // set iterator on last element :(
    file_lists_iterator_ = file_lists_.end();
    --file_lists_iterator_;
}
