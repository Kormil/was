#ifndef ASYNCIMAGEPROVIDER_H
#define ASYNCIMAGEPROVIDER_H

#include <memory>

#include <QQuickImageProvider>
#include <QImage>
#include <QString>

#include "src/connection/controller.h"

class AsyncImageResponse : public QQuickImageResponse {
    Q_OBJECT

signals:
    void getThumbnailSignal(int id, const QString &cacheKey, const QString &type);
    void getImageSignal(int id, const QString &cacheKey, const QString &type);

public:
    AsyncImageResponse(const QString &cache_key, const QSize &requested_size, const QString &type) :
        cache_key_(cache_key),
        requested_size_(requested_size),
        type_(type)
    {
        id_ = cache_key.split(u'_', QString::SkipEmptyParts).first();
    }

    void getThumbnail() {
        emit getThumbnailSignal(id_.toInt(), cache_key_, type_);
    }

    void getImage() {
        emit getImageSignal(id_.toInt(), cache_key_, type_);
    }

    void onGetThumbnailFinished(const QString &cache_key, const QImage& image) {
        if (cache_key != cache_key_) {
            return;
        }

        if (image.width() == image.height()) {
            image_ = image;
        } else if (image.width() < image.height()){
            auto y = (image.height() - image.width()) / 2.0;

            image_ = image.copy(0, y, image.width(), image.width());
        } else {
            auto x = (image.width() - image.height()) / 2.0;

            image_ = image.copy(x, 0, image.height(), image.height());
        }

        emit finished();
    }

    void onGetImageFinished(const QString &cache_key, const QImage& image) {
        if (cache_key != cache_key_) {
            return;
        }

        image_ = image;
        emit finished();
    }

    QQuickTextureFactory *textureFactory() const override {
        return QQuickTextureFactory::textureFactoryForImage(image_);
    }

private:
    QString id_;
    QString cache_key_;
    QImage image_;
    QSize requested_size_;
    QString type_;
};

class AsyncPhotoProvider : public QQuickAsyncImageProvider
{
public:
    AsyncPhotoProvider(Controller &controller) :
        controller_(controller)
    {}

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override
    {
        AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize, "unit");

        QObject::connect(&controller_, &Controller::onGetImageFinished, response, &AsyncImageResponse::onGetImageFinished);
        QObject::connect(response, &AsyncImageResponse::getImageSignal, &controller_, &Controller::getImage);

        response->getImage();

        return response;
    }

private:
    Controller &controller_;
};

class AsyncThumbnailProvider : public QQuickAsyncImageProvider
{
public:
    AsyncThumbnailProvider(Controller &controller) :
        controller_(controller)
    {}

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override
    {
        AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize, "unit");

        QObject::connect(&controller_, &Controller::onGetThumbnailFinished, response, &AsyncImageResponse::onGetThumbnailFinished);
        QObject::connect(response, &AsyncImageResponse::getThumbnailSignal, &controller_, &Controller::getThumbnail);

        response->getThumbnail();

        return response;
    }

private:
    Controller &controller_;
};

class AsyncFolderProvider : public QQuickAsyncImageProvider
{
public:
    AsyncFolderProvider(Controller &controller) :
        controller_(controller)
    {}

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override
    {
        AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize, "folder");

        QObject::connect(&controller_, &Controller::onGetThumbnailFinished, response, &AsyncImageResponse::onGetThumbnailFinished);
        QObject::connect(response, &AsyncImageResponse::getThumbnailSignal, &controller_, &Controller::getThumbnail);

        response->getThumbnail();

        return response;
    }

private:
    Controller &controller_;
};

#endif // ASYNCIMAGEPROVIDER_H
