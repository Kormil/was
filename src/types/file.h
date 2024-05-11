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
    virtual QString cacheKey() const { return ""; };
    virtual QString createTime() const  { return ""; };

    virtual void setCount(int) {};
    virtual int count() const { return 0; };;

    virtual void fromJson(QJsonObject& json) = 0;
};

#endif // FILE_H
