#ifndef LOGINREQUEST_H
#define LOGINREQUEST_H

#include <functional>

#include "src/connection/request.h"

class LoginRequest : public Request {
public:
    using HandlerType = std::function<void (bool, const QString& sid)>;

    LoginRequest(const QString &quickconnect, const QString &login, const QString &password, HandlerType handler);
    std::pair<bool, QString> parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //LOGINREQUEST_H
