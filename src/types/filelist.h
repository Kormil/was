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

    using DataType = std::vector<FilePtr>;
public:
    explicit FileList(QObject *qobject = nullptr);
    virtual ~FileList();

    std::size_t size() const;

    IdType id() const;
    void setId(IdType id);

    FilePtr getByIndex(int index);
    FilePtr get(const IdType& id);
    bool has(const IdType& id);

    void setParentFile(const FilePtr& parent);
    const FilePtr& parentFile();

    int index(const IdType& id) const;

    void append(const FilePtr &file);
    void appendList(const FileListPtr &files);

    std::vector<IdType> getAllIds() const;

    void setItemsCount(int count);
    void setFoldersCount(int count);
    unsigned int countItems() const;
    unsigned int countAllItems() const;
    bool canFetchMore() const;

    QVariant data(const IdType file_id, int role);
    void setData(const IdType id, int role, QVariant value);

    DataType::const_iterator begin() const { return files_.begin(); }
    DataType::const_iterator end() const { return files_.end(); }

    unsigned int already_downloaded_files_ = 0;
signals:
    void preItemAppended(int);
    void postItemAppended();
    void dataChanged(int, int);
    void countChanged(int);

private:
    unsigned int all_files_counter_ = 0;
    FilePtr parent_;

    IdType id_ = 0;
    DataType files_;
    std::map<IdType, int> id_to_row_;
};

#endif // FILELIST_H
