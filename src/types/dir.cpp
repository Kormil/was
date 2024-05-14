#include "dir.h"

#include <QDebug>
#include <QJsonArray>

QString Dir::name() const {
    return name_;
}

IdType Dir::id() const {
    return id_;
}

IdType Dir::parentId() const {
    return parent_id_;
}

void Dir::fromJson(QJsonObject &json) {
    id_ = json["id"].toInt();
    path_ = json["name"].toString();
    name_ = path_.section("/", -1, -1);
    parent_id_ = json["parent"].toInt();

    auto thumbnails = json["additional"].toObject()["thumbnail"].toArray();
    if (thumbnails.size()) {
        auto thumbnail = thumbnails[0];
        if (thumbnail.isObject()) {
            cache_key_ = thumbnail.toObject()["cache_key"].toString();

            if (!cache_key_.contains("_")) {
                cache_key_ = QString::number(id_) + "_" + cache_key_;
                has_thumbnail_ = true;
            }
        }
    }
}

void Dir::setCount(int count) {
    count_ = count;
}

int Dir::count() const {
    return count_;
}

QString Dir::cacheKey() const {
    if (!has_thumbnail_ && cache_key_.isEmpty()) {
        return "image://theme/icon-m-file-folder";
    }

    if (!has_thumbnail_) {
        return "image://thumbnails/" + cache_key_;
    }

    return "image://folders/" + cache_key_;
}
