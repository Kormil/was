#include "file.h"

QString File::name() const {
    return name_;
}

IdType File::id() const {
    return id_;
}

IdType File::parentId() const {
    return parent_id_;
}

void File::fromJson(QJsonObject &json) {
    id_ = json["id"].toInt();
    name_ = json["name"].toString();
    parent_id_ = json["parent"].toInt();
}
