#include "loginrequest.h"

#include <QJsonDocument>
#include <QJsonObject>

LoginRequest::LoginRequest(const QString &quickconnect, const QString &login, const QString &password, HandlerType handler) :
    Request()
{
    add_sid_ = false;

    api_ = "auth.cgi?api=SYNO.API.Auth";
    parameters_ = QString("version=3&method=login&account=%2&passwd=%3&format=sid").arg(login).arg(password);

    QObject::connect(this, &Request::ready, [this, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            qDebug() << "Connection error";
            handler(false, "");
        } else {
            qDebug() << responseArray;
            auto response = parseAnswer(QJsonDocument::fromJson(responseArray));
            handler(response.first, response.second);
        }

        emit done(serial());
    });
}

std::pair<bool, QString> LoginRequest::parseAnswer(const QJsonDocument &jsonDocument) {
    if (jsonDocument.isNull())
        return {false, ""};

    const auto& object = jsonDocument.object();
    const auto& data = object["data"];
    if (data.isUndefined()) {
        return {false, ""};
    }

    const auto& data_object = data.toObject();

    auto success = object["success"].toBool();
    auto sid = data_object["sid"].toString();

    return {success, sid};
}
