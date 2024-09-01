#include "filesmanager.h"

#include <algorithm>
#include <cassert>

#include "src/settings.h"
#include "src/models/filelistmodel.h"
#include "src/connection/requests/countfoldersrequest.h"
#include "src/connection/requests/countitemsrequest.h"
#include "src/connection/requests/getrootfoldersrequest.h"
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

void FilesManager::getItemsCounterForParent(const FileListPtr &files) {
    auto space = Settings::instance()->apiSpace();

    // items
    auto count_items_request = std::make_shared<CountItemsRequest>(files->id(), space, [files](int, int count) {
        files->setItemsCount(count);
    });

    auto count_items_serial = connection_->addRequest(count_items_request);
    connection_->runRequest(count_items_serial);

    // folders
    auto count_folders_request = std::make_shared<CountFoldersRequest>(files->id(), space, [files](int, int count) {
        files->setFoldersCount(count);
    });
    auto count_folders_serial = connection_->addRequest(count_folders_request);
    connection_->runRequest(count_folders_serial);
}

void FilesManager::getItemsCounter(const FileListPtr &files) {
    auto space = Settings::instance()->apiSpace();

    for (const auto& file: *files) {
        if (file->isDir()) {
            auto count_items_request = std::make_shared<CountItemsRequest>(file->id(), space, [files](int subdir_id, int count) {
                files->setData(subdir_id, FileListModel::FileListRole::ItemsCounterRole, count);
            });

            auto count_items_serial = connection_->addRequest(count_items_request);
            connection_->runRequest(count_items_serial);

            auto count_folders_request = std::make_shared<CountFoldersRequest>(file->id(), space, [files](int subdir_id, int count) {
                files->setData(subdir_id, FileListModel::FileListRole::FoldersCounterRole, count);
            });

            auto count_folders_serial = connection_->addRequest(count_folders_request);
            connection_->runRequest(count_folders_serial);
        }
    }
}

void FilesManager::cleanCurrentStack(int index)
{
    // it is last item
    if (index == file_lists_.size() - 1) {
        return;
    }

    file_lists_iterator_ = file_lists_.erase(file_lists_.begin() + index + 1, file_lists_.end());
    --file_lists_iterator_;
}

void FilesManager::getItems(IdType folder_id, std::function<void(const FileListPtr &files)> handler) {
    std::lock_guard lock{file_list_iterator_mutex_};

    // Search where put images
    qDebug() << "REQUEST IMAGES FOR: " << folder_id;
    auto index = findLeaf(folder_id);

    // Do we have everything already?
    if (file_lists_[index].items && !file_lists_[index].items->canFetchMore()) {
        return handler(file_lists_[index].items);
    }

    // Let's get more
    return fetchMoreItems(folder_id, 0, handler);
}

void FilesManager::getFolders(IdType id, std::function<void(const FileListPtr &files)> handler) {
    std::lock_guard lock{file_list_iterator_mutex_};

    {
        std::lock_guard lock{get_folders_mtx_};
        auto found = get_folders_handlers_.find(id);
        bool return_after_insert = found != get_folders_handlers_.end();

        get_folders_handlers_.insert({id, handler});

        if (return_after_insert) {
            return;
        }
    }

    auto space = Settings::instance()->apiSpace();
    auto get_folders_handler = [this, id](const FileListPtr &files) {
        auto all_files = appendOrInsertToFolders(id, files);

        {
            std::lock_guard lock{get_folders_mtx_};
            auto range = get_folders_handlers_.equal_range(id);
            for (auto handler = range.first; handler != range.second; ++handler) {
                handler->second(all_files);
            }
            get_folders_handlers_.erase(id);
        }
    };

    auto request = std::make_shared<GetFoldersRequest>(id, space, get_folders_handler);


    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

void FilesManager::getRootFolder(GetRootFoldersHandlerType handler) {
//    std::lock_guard lock{file_list_iterator_mutex_};

//    {
//        std::lock_guard lock{get_folders_mtx_};
//        bool return_after_insert = !get_root_folder_handlers_.empty();

//        get_root_folder_handlers_.push_back(handler);

//        if (return_after_insert) {
//            return;
//        }
//    }

//    auto space = Settings::instance()->apiSpace();
//    auto get_folders_handler = [this](int downloaded_folder_id, const FileListPtr &files) {
//        auto all_files = appendOrInsertToFolders(downloaded_folder_id, files);

//        {
//            std::lock_guard lock{get_folders_mtx_};
//            for (auto &handler : get_root_folder_handlers_) {
//                handler(downloaded_folder_id, all_files);
//            }
//            get_root_folder_handlers_.clear();
//        }
//    };

//    auto request = std::make_shared<GetRootFoldersRequest>(space, get_folders_handler);

//    auto serial = connection_->addRequest(request);
//    connection_->runRequest(serial);
}

void FilesManager::fetchMoreItems(IdType id, unsigned int start_point, GetItemsHandlerType handler) {
    {
        std::lock_guard lock{get_items_mtx_};
        auto search = get_items_handlers_.find(id);
        bool return_after_insert = search != get_items_handlers_.end();

        get_items_handlers_.insert({id, handler});

        if (return_after_insert) {
            return;
        }
    }

    auto space = Settings::instance()->apiSpace();
    auto request = std::make_shared<GetItemsRequest>(id, start_point, space, [this, id](const FileListPtr &files) {
        auto all_files = appendOrInsertToItems(id, files);

        {
            std::lock_guard lock{get_items_mtx_};
            auto range = get_items_handlers_.equal_range(id);
            for (auto handler = range.first; handler != range.second; ++handler) {
                handler->second(all_files);
            }
            get_items_handlers_.erase(id);
        }
    });

    auto serial = connection_->addRequest(request);
    connection_->runRequest(serial);
}

FilePtr FilesManager::getFile(IdType file_id) {
    if (!file_id) {
        return nullptr;
    }

    for (auto& file_list: file_lists_) {
        if (!file_list.folders) {
            continue;
        }

        for (const auto &file: *file_list.folders) {
            if (file->id() == file_id) {
                return file;
            }
        }
    }

    return nullptr;
}

int FilesManager::findLeaf(int folder_id) {
    for (std::size_t i = 0; i < file_lists_.size(); ++i) {
        if (file_lists_[i].id == folder_id) {
            return i;
        }
    }

    Leaf l;
    l.id = folder_id;

    int index = file_lists_.size();
    file_lists_.push_back(l);

    // set iterator on last element :(
    file_lists_iterator_ = file_lists_.end();
    --file_lists_iterator_;

    return index;
}

int FilesManager::findParentLeaf(int folder_id) {
    for (int i = file_lists_.size() - 1; i >= 0; --i) {
        if (file_lists_[i].folders && file_lists_[i].folders->has(folder_id)) {
            return i;
        }
    }

    return -1;
}

FileListPtr FilesManager::appendOrInsertToItems(int folder_id, const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};

    // new items
    const auto &parent = getFile(folder_id);
    if (parent) {
        files->setParentFile(parent);
    }

    auto index = findLeaf(folder_id);
    //assert(("No exists leaf for folder: " + folder_id, file_lists_[index].parent_index == -1));

    auto& list = file_lists_[index].items;
    if (list) {
        list->appendList(files);
    } else {
        list = files;
    }

    return list;
}

FileListPtr FilesManager::appendOrInsertToFolders(int folder_id, const FileListPtr &files) {
    std::lock_guard lock{file_list_iterator_mutex_};


    // new items
    const auto &parent = getFile(folder_id);
    if (parent) {
        files->setParentFile(parent);
    }

    auto parent_index = findParentLeaf(folder_id);
    //assert(("No exists parent for folder: " + folder_id, parent_index == -1));
    cleanCurrentStack(parent_index);

    auto index = findLeaf(folder_id);

    file_lists_[index].index = index;
    file_lists_[index].parent_index = parent_index;

    auto& list = file_lists_[index].folders;
    if (list) {
        list->appendList(files);
    } else {
        list = files;
    }

    return list;
}
