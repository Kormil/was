#ifndef SHOWPICTUREMODEL_H
#define SHOWPICTUREMODEL_H

#include <memory>

#include "src/models/filelistmodel.h"

class ShowPictureModel : public FileListModel
{
    Q_OBJECT
    Q_PROPERTY(int current_index READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(QString current_picture_source READ currentPictureSource NOTIFY currentPictureSourceChanged)

public:
    explicit ShowPictureModel(QObject *parent = nullptr);

    int currentIndex() const;
    void setCurrentIndex(int value);

    QString currentPictureSource();

signals:
    void currentPictureSourceChanged();

protected:
    bool canFetchMore(const QModelIndex &parent) const override;

private:
    int current_index_;
};

using ShowPictureModelPtr = std::shared_ptr<ShowPictureModel>;

#endif // SHOWPICTUREMODEL_H
