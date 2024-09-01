#ifndef FILE_H
#define FILE_H

#include <memory>

#include <QObject>
#include <QJsonObject>

class File;
using FilePtr = std::shared_ptr<File>;

using IdType = int;

class File : public QObject {

public:
    virtual IdType id() const = 0;
    virtual IdType parentId() const = 0;
    virtual QString name() const = 0;

    virtual bool isDir() const = 0;
    virtual QString photoSource() const { return ""; };
    virtual QString cacheKey() const  = 0;
    virtual QString createTime() const  { return ""; };

    virtual void setItemsCount(int count) {};
    virtual void setFoldersCount(int count) {};
    virtual int countItems() const { return 0; };
    virtual int countAllItems() const { return 0; };

    virtual void fromJson(QJsonObject& json) = 0;
};

#endif // FILE_H
