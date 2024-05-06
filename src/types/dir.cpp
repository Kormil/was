#include "dir.h"

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
}
