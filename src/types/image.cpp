#include "image.h"

 #include <QDateTime>

#include "src/connection/controller.h"

QString Image::name() const {
    return name_;
}

IdType Image::id() const {
    return id_;
}

IdType Image::parentId() const {
    return parent_id_;
}

QString Image::cacheKey() const {
    return "image://thumbnails/" + cache_key_;
}

QString Image::photoSource() const {
    return "image://photos/" + cache_key_;
}

QString Image::createTime() const {
    return create_time_;
}

void Image::fromJson(QJsonObject &json) {
    id_ = json["id"].toInt();
    name_ = json["filename"].toString();
    parent_id_ = json["parent"].toInt();

    long long create_time_msec = json["time"].toInt();
    create_time_msec = create_time_msec * 1000;
    create_time_ = QDateTime::fromMSecsSinceEpoch(create_time_msec).toString();

    auto thumbnail = json["additional"].toObject()["thumbnail"].toObject();
    cache_key_ = thumbnail["cache_key"].toString();
}
