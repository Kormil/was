﻿#include "countitemsrequest.h"

#include <QJsonDocument>

CountItemsRequest::CountItemsRequest(IdType id, Request::Space space, HandlerType handler) :
    Request()
{
    if (space == Request::PERSONAL) {
        api_ = "entry.cgi?api=SYNO.Foto.Browse.Item";
    } else {
        api_ = "entry.cgi?api=SYNO.FotoTeam.Browse.Item";
    }

    parameters_ = QString("version=1&method=count&folder_id=%1").arg(id);

    QObject::connect(this, &Request::ready, [this, id, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            qDebug() << "Connection error";
            handler(id, 0);
        } else {
            auto response = parseAnswer(QJsonDocument::fromJson(responseArray));
            handler(id, response);
        }

        emit done(serial());
    });
}

int CountItemsRequest::parseAnswer(const QJsonDocument &jsonDocument) {
    if (jsonDocument.isNull()) {
        return 0;
    }

    const auto& object = jsonDocument.object();
    const auto& data = object["data"];
    if (data.isUndefined()) {
        return 0;
    }

    const auto& data_object = data.toObject();
    const auto& count = data_object["count"].toInt();
    return count;
}
