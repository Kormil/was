#ifndef FILE_H
#define FILE_H

#include <memory>

#include <QObject>
#include <QJsonObject>
#include <QString>

class File;
using FilePtr = std::shared_ptr<File>;

using IdType = int;

class File : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
    File()  = default;
    File& operator=(const File&) = default;
    File& operator=(File&&) = default;

    void fromJson(QJsonObject& json);

    IdType id() const;
    IdType parentId() const;
    QString name() const;

signals:
    void idChanged();
    void nameChanged();

private:
    IdType id_;
    IdType parent_id_;
    QString name_;
};

#endif // FILE_H
