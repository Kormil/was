#include "logoutrequest.h"

LogoutRequest::LogoutRequest(HandlerType handler) :
    Request()
{
    url_ = QString("/webapi/auth.cgi?api=SYNO.API.Auth&version=1&method=logout");

    QObject::connect(this, &Request::ready, [this, handler](Request::Status status, const QByteArray& responseArray) {
        handler();
        emit done(serial());
    });
}
