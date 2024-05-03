#include "image.h"

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
    return cache_key_;
}

void Image::fromJson(QJsonObject &json) {
    id_ = json["id"].toInt();
    name_ = json["filename"].toString();
    parent_id_ = json["parent"].toInt();

    auto thumbnail = json["additional"].toObject()["thumbnail"].toObject();
    cache_key_ = thumbnail["cache_key"].toString();
}
