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

public:
    FilesManager(const ConnectionPtr& connection);

    std::size_t size() const;

    void pop();

    void get(IdType id, unsigned int start_point, std::function<void(const FileListPtr &files)> handler);
    FilePtr getFile(IdType file_id);

    void getItemsCounter(const FileListPtr &files);

    void clear();

private:
    void append(const FileListPtr &files);
    void fetchMoreItems(IdType id, unsigned int start_point, std::function<void(const FileListPtr &files)> handler);

    std::mutex file_list_iterator_mutex_;

    ConnectionPtr connection_;

    std::vector<std::pair<IdType, FileListPtr>>::iterator file_lists_iterator_;
    std::vector<std::pair<IdType, FileListPtr>> file_lists_;

};

#endif // FILESMANAGER_Hs
