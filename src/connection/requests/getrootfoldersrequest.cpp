#include "getrootfoldersrequest.h"

#include <QJsonDocument>
#include <QJsonArray>

#include "src/types/dir.h"

GetRootFoldersRequest::GetRootFoldersRequest(Request::Space space, HandlerType handler) :
    Request()
{
    if (space == Request::PERSONAL) {
        api_ = "entry.cgi?api=SYNO.Foto.Browse.Folder";
    } else {
        api_ = "entry.cgi?api=SYNO.FotoTeam.Browse.Folder";
    }

    parameters_ = QString("version=2&method=list_parents");

    QObject::connect(this, &Request::ready, [this, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            qDebug() << "Connection error";
            handler(0);
        } else {
            auto response = parseAnswer(QJsonDocument::fromJson(responseArray));
            handler(response->id());
        }

        emit done(serial());
    });
}

FileListPtr GetRootFoldersRequest::parseAnswer(const QJsonDocument &jsonDocument) {
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
        auto dir = std::make_shared<Dir>();
        auto dirJson = result.toObject();
        dir->fromJson(dirJson);

        if (dir->name() == "") {
            files->setParentFile(dir);
            
            continue;
        }

        files->append(dir);
    }

    return std::move(files);
}
