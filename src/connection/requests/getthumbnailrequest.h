#ifndef GETTHUMBNAILREQUEST_H
#define GETTHUMBNAILREQUEST_H

#include <functional>

#include <QImage>

#include "src/types/image.h"
#include "src/connection/request.h"

class GetThumbnailRequest : public Request {
public:
    using HandlerType = std::function<void (const QImage&)>;

    GetThumbnailRequest(Image::Size size, IdType id, QString cacheKey, QString type, Request::Space space, HandlerType handler);
};


#endif //GETTHUMBNAILREQUEST_H
