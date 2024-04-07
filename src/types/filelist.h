#ifndef FILELIST_H
#define FILELIST_H

#include <memory>
#include <map>

#include <QObject>
#include <QString>

#include "file.h"

class FileList;
using FileListPtr = std::shared_ptr<FileList>;


class FileList : public QObject {
    Q_OBJECT

public:
    explicit FileList(QObject *parent = nullptr);
    virtual ~FileList();

    std::size_t size() const;

    FilePtr getByIndex(int index);
    FilePtr get(const IdType& id);

    int index(const IdType& id) const;

    void append(const FilePtr &file);
    void appendList(FileListPtr &files);

signals:
    void preItemAppended();
    void postItemAppended();

private:
    std::vector<FilePtr> files_;
    std::map<IdType, int> id_to_row_;
};

#endif // FILELIST_H
