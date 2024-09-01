#ifndef FILESMANAGER_H
#define FILESMANAGER_H

#include <memory>
#include <vector>
#include <mutex>

#include "filelist.h"
#include "src/connection/connection.h"

class FilesManager;
using FilesManagerPtr = std::unique_ptr<FilesManager>;

class FilesManager {

    struct Leaf {
        IdType id;

        int index = -1;
        int parent_index= -1;

        FileListPtr items;
        FileListPtr folders;
    };

public:
    using GetItemsHandlerType = std::function<void(const FileListPtr &files)>;
    using GetFoldersHandlerType = std::function<void(const FileListPtr &files)>;
    using GetRootFoldersHandlerType = std::function<void(int, const FileListPtr &files)>;

    FilesManager(const ConnectionPtr& connection);

    std::size_t size() const;

    void pop();

    void getItems(IdType folder_id, GetItemsHandlerType handler);
    void getFolders(IdType id, GetFoldersHandlerType handler);
    void getRootFolder(GetRootFoldersHandlerType handler);

    FilePtr getFile(IdType file_id);

    void getItemsCounterForParent(const FileListPtr &file);
    void getItemsCounter(const FileListPtr &files);

    void clear();

private:
    int findLeaf(int folder_id);
    int findParentLeaf(int folder_id);

    FileListPtr appendOrInsertToFolders(int folder_id, const FileListPtr &files);
    FileListPtr appendOrInsertToItems(int folder_id, const FileListPtr &files);

    void fetchMoreItems(IdType id, unsigned int start_point, GetItemsHandlerType handler);

    std::mutex file_list_iterator_mutex_;

    ConnectionPtr connection_;

    std::mutex get_folders_mtx_;
    std::mutex get_items_mtx_;
    std::vector<GetRootFoldersHandlerType> get_root_folder_handlers_;
    std::multimap<IdType, GetFoldersHandlerType> get_folders_handlers_;
    std::multimap<IdType, GetItemsHandlerType> get_items_handlers_;

    std::vector<Leaf>::iterator file_lists_iterator_;
    std::vector<Leaf> file_lists_;

    void cleanCurrentStack(int index);
};

#endif // FILESMANAGER_Hs
