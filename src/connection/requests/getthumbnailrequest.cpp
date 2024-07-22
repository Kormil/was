#include "getthumbnailrequest.h"

#include <QJsonDocument>

GetThumbnailRequest::GetThumbnailRequest(Image::Size size, IdType id, QString cacheKey, QString type, HandlerType handler) :
    Request()
{
    QString size_text;
    switch (size) {
        case Image::Size::ORIGINAL: size_text = "original"; break;
        case Image::Size::SM: size_text = "sm"; break;
        case Image::Size::M: size_text = "m"; break;
        case Image::Size::XL: size_text = "xl"; break;
    }

    url_ = QString("/webapi/entry.cgi?api=SYNO.FotoTeam.Thumbnail&method=get&version=2&id=%1&cache_key=%2&type=%3&size=%4").arg(id).arg(cacheKey).arg(type).arg(size_text);

    QObject::connect(this, &Request::ready, [this, handler](Request::Status status, const QByteArray& responseArray) {
        QImage pixmap;

        if (status == Request::ERROR) {
            qDebug() << "Connection error";
            handler(pixmap);
        } else {
            pixmap.loadFromData(responseArray);
            handler(pixmap);
        }

        emit done(serial());
    });
}
