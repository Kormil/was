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

void FilesManager::getItemsCounter(int id, const FileListPtr &files)
{
    auto handler = [this, id](int subdir_id, int count) {
        if (!count ) {
            return;
        }

        for (const auto& id_files_pair: file_lists_) {
            if (id_files_pair.first == id) {
                auto& files = id_files_pair.second;
                auto file = files->get(subdir_id);

                count += file->count();
                files->setData(subdir_id, FileListModel::FileListRole::ItemCounterRole, count);
            }
        }
    };

    const auto& subdir_ids = files->getAllIds();
    for (const auto& subdir_id: subdir_ids) {
        connection_->countOfDirectoryItems(subdir_id, handler);
        connection_->countOfDirectory(subdir_id, handler);
    }
}

void FilesManager::get(IdType id, std::function<void(const FileListPtr &files)> handler) {
    const auto next_id = getNextId();

    std::lock_guard lock{file_list_iterator_mutex_};

    if (next_id != InvalidId) {
        ++file_lists_iterator_;
        return handler(top());
    }

    file_lists_.erase(file_lists_iterator_, file_lists_.end());


    connection_->contentOfDirectory(id, [this, id, handler](const FileListPtr &files) {
        append(id, files);
        getItemsCounter(id, files);
        handler(file_lists_iterator_->second);
    });

    connection_->contentOfDirectoryItems(id, [this, id, handler](const FileListPtr &files) {
        append(id, files);
        handler(file_lists_iterator_->second);
    });

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

void FilesManager::append(IdType id, const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};

    // got something already
    if (file_lists_iterator_ != file_lists_.end()) {
        if (file_lists_iterator_->first == id) {
            file_lists_iterator_->second->appendList(files);
            return;
        }
    }

    // new items
    file_lists_.push_back({id, files});

    // set iterator on last element :(
    file_lists_iterator_ = file_lists_.end();
    --file_lists_iterator_;
}
