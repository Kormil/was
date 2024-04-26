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
    name_ = json["name"].toString();
    parent_id_ = json["parent"].toInt();
}
