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

    qDebug() << rowCount() << " < " << current_index_ + 5;
    return rowCount() < current_index_ + 5;
}

int ShowPictureModel::currentIndex() const {
    return current_index_;
}

void ShowPictureModel::setCurrentIndex(int value) {
    qDebug() << value;
    current_index_ = value;
}
