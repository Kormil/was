#ifndef DIR_H
#define DIR_H

#include <memory>

#include <QObject>
#include <QJsonObject>
#include <QString>

#include "file.h"

class Dir;
using DirPtr = std::shared_ptr<Dir>;

class Dir : public File {
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
    Dir()  = default;
    Dir& operator=(const Dir&) = default;
    Dir& operator=(Dir&&) = default;

    void fromJson(QJsonObject& json) override;

    IdType id() const override;
    IdType parentId() const override;
    QString name() const override;
    QString cacheKey() const;

    bool isDir() const override {
        return true;
    }

    void setCount(int count) override;
    int count() const override;

signals:
    void idChanged();
    void nameChanged();
    void urlChanged();

private:
    IdType id_;
    IdType parent_id_;
    QString name_;
    QString path_;
    QString cache_key_;
    int count_;

    bool has_thumbnail_ = false;
};

#endif // Dir_H
