#include "getitemsrequest.h"

#include <QJsonDocument>
#include <QJsonArray>

#include "src/types/image.h"

GetItemsRequest::GetItemsRequest(IdType id, unsigned int start_point, HandlerType handler) :
    Request()
{
    url_ = QString("/webapi/entry.cgi?api=SYNO.FotoTeam.Browse.Item&version=1&method=list&offset=%1&limit=100&folder_id=%2&additional=[%22thumbnail%22]").arg(start_point).arg(id);

    QObject::connect(this, &Request::ready, [this, id, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            qDebug() << "Connection error";
            handler(nullptr);
        } else {
            auto response = parseAnswer(QJsonDocument::fromJson(responseArray));
            response->setId(id);

            handler(response);
        }

        emit done(serial());
    });
}

FileListPtr GetItemsRequest::parseAnswer(const QJsonDocument &jsonDocument) {
    if (jsonDocument.isNull()) {
        return nullptr;
    }

    FileListPtr files = std::make_shared<FileList>();

    const auto& object = jsonDocument.object();
    const auto& data = object["data"];
    if (data.isUndefined()) {
        return files;
    }

    const auto& data_object = data.toObject();
    const auto& list = data_object["list"];
    if (list.isUndefined()) {
        return files;
    }

    QJsonArray results = list.toArray();

    for (const auto& result: results) {
        auto image = std::make_shared<Image>();
        auto imageJson = result.toObject();
        image->fromJson(imageJson);

        files->append(image);
    }

    return std::move(files);
}
