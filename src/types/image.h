#ifndef IMAGE_H
#define IMAGE_H

#include <memory>

#include <QObject>
#include <QJsonObject>
#include <QString>

#include "file.h"

class Image;
using ImagePtr = std::shared_ptr<Image>;

class Image : public File {
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString cache_key READ cacheKey NOTIFY cacheKeyChanged)

public:
    enum Size {
        ORIGINAL,
        SM,
        M,
        XL
    };

    Image()  = default;
    Image& operator=(const Image&) = default;
    Image& operator=(Image&&) = default;

    void fromJson(QJsonObject& json) override;

    IdType id() const;
    IdType parentId() const;
    QString name() const;
    QString cacheKey() const;
    QString photoSource() const;
    QString createTime() const;

    bool isDir() const override {
        return false;
    }

signals:
    void idChanged();
    void nameChanged();
    void cacheKeyChanged();

private:
    IdType id_;
    IdType parent_id_;
    QString name_;
    QString cache_key_;
    QString create_time_;
};

#endif // IMAGE_H
