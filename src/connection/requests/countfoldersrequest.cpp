#include "countfoldersrequest.h"

#include <QJsonDocument>

CountFoldersRequest::CountFoldersRequest(IdType id, Request::Space space, HandlerType handler) :
    Request()
{
    if (space == Request::PERSONAL) {
        api_ = "entry.cgi?api=SYNO.Foto.Browse.Folder";
    } else {
        api_ = "entry.cgi?api=SYNO.FotoTeam.Browse.Folder";
    }

    parameters_ = QString("version=2&method=count&offset=0&id=%1&additional=[%22thumbnail%22]").arg(id);

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

int CountFoldersRequest::parseAnswer(const QJsonDocument &jsonDocument) {
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
