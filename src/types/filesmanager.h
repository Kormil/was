#ifndef FILESMANAGER_H
#define FILESMANAGER_H

#include <memory>
#include <vector>

#include "filelist.h"
#include "src/connection/connection.h"

class FilesManager;
using FilesManagerPtr = std::unique_ptr<FilesManager>;

class FilesManager {

public:
    FilesManager(const ConnectionPtr& connection);

    std::size_t size() const;

    FileListPtr top();
    void pop();

    void get(IdType id, std::function<void(const FileListPtr &files)> handler);

    void append(IdType id, const FileListPtr &files);

private:
    IdType getNextId();
    void append(const FileListPtr &files);

    ConnectionPtr connection_;

    std::vector<std::pair<IdType, FileListPtr>>::iterator file_lists_iterator_;
    std::vector<std::pair<IdType, FileListPtr>> file_lists_;
};

#endif // FILESMANAGER_Hs
