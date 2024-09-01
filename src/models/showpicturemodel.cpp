#include "showpicturemodel.h"

#include "src/connection/controller.h"

ShowPictureModel::ShowPictureModel(QObject *parent)
    : FileListModel(parent)
{
}

bool ShowPictureModel::canFetchMore(const QModelIndex &parent) const
{
    if (!FileListModel::canFetchMore(parent)) {
        return false;
    }

    return rowCount() < current_index_ + 5;
}

int ShowPictureModel::currentIndex() const {
    return current_index_;
}

void ShowPictureModel::setCurrentIndex(int value) {
    current_index_ = value;

    emit currentPictureSourceChanged();
}

QString ShowPictureModel::currentPictureSource() {
    return index(current_index_, 0).data(FileListRole::PhotoSourceRole).toString();
}
