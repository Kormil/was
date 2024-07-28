#include "logoutrequest.h"

LogoutRequest::LogoutRequest(HandlerType handler) :
    Request()
{
    api_ = "auth.cgi?api=SYNO.API.Auth";
    parameters_ = QString("version=1&method=logout");

    QObject::connect(this, &Request::ready, [this, handler](Request::Status status, const QByteArray& responseArray) {
        handler();
        emit done(serial());
    });
}
