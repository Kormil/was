#include "filelist.h"

#include <algorithm>

FileList::FileList(QObject *parent) : QObject(parent) {

}


FileList::~FileList() {
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
        emit preItemAppended();

        int row = files_.size();
        files_.push_back(file);
        id_to_row_[file->id()] = row;

        emit postItemAppended();
    }
}

void FileList::appendList(FileListPtr &files)
{
    for (auto& file: files->files_) {
        append(file);
    }

    files = nullptr;
}
