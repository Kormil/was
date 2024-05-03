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
    void getThumbnail(int id, const QString &cacheKey);

    public:
        AsyncImageResponse(const QString &cache_key, const QSize &requestedSize) :
            cache_key_(cache_key) {
            id_ = cache_key.split(u'_', QString::SkipEmptyParts).first();
         }

        void start() {
            emit getThumbnail(id_.toInt(), cache_key_);
        }

        void onGetThumbnailFinished(const QString &cache_key, const QImage& image) {
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
};

class AsyncImageProvider : public QQuickAsyncImageProvider
{
public:
    AsyncImageProvider(Controller &controller) :
    controller_(controller)
    {}

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override
    {
        AsyncImageResponse *response = new AsyncImageResponse(id, requestedSize);

        QObject::connect(&controller_, &Controller::onGetThumbnailFinished, response, &AsyncImageResponse::onGetThumbnailFinished);
        QObject::connect(response, &AsyncImageResponse::getThumbnail, &controller_, &Controller::getThumbnail);

        response->start();

        return response;
    }

private:
    Controller &controller_;
};

#endif // ASYNCIMAGEPROVIDER_H
