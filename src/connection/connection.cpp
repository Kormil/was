#include "connection.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <QPixmap>

#include "request.h"

Connection::Connection()
{
    m_networkAccessManager = std::make_unique<QNetworkAccessManager>();
    auto diskCache = new QNetworkDiskCache(m_networkAccessManager.get());
    diskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                                 .append("/network"));
    m_networkAccessManager->setCache(diskCache);
}

void Connection::deleteRequest(int serial)
{
    std::lock_guard<std::mutex> lock(m_networkRequestsMutex);
    m_networkRequests.erase( m_networkRequests.find(serial) );
}

QNetworkAccessManager* Connection::networkAccessManager()
{
    return m_networkAccessManager.get();
}

int Connection::nextSerial()
{
    m_serial = m_serial + 1;
    return m_serial;
}

void Connection::clearRequests()
{
    m_networkAccessManager->deleteLater();
}

Request* Connection::request(const QUrl &requestUrl)
{
    std::lock_guard<std::mutex> lock(m_networkRequestsMutex);

    int serial = nextSerial();

    RequestPtr requestPtr = std::make_unique<Request>(requestUrl, this);

    requestPtr->setSerial(serial);
    m_networkRequests[serial] = std::move(requestPtr);

    return m_networkRequests[serial].get();
}

void Connection::login(QString quickconnect, QString login, QString password, std::function<void (bool)> handler) {

    QString parsedParameters;

    QString url = QString("https://%1.fr.quickconnect.to/webapi/auth.cgi?api=SYNO.API.Auth&version=3&method=login&account=%2&passwd=%3&format=sid").arg(quickconnect).arg(login).arg(password);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    parameters_.quickconnect = quickconnect;

    QObject::connect(requestRaw, &Request::finished, [this, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(false);
        } else {
            auto response = parseLoginAnswer(QJsonDocument::fromJson(responseArray));
            handler(response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

void Connection::logout(std::function<void ()> handler) {

    QString parsedParameters;

    QString url = QString("https://%1.fr.quickconnect.to/webapi/auth.cgi?api=SYNO.API.Auth&version=3&method=logout").arg(parameters_.quickconnect);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        handler();
        deleteRequest(requestRaw->serial());
    });

    requestRaw->run();
}

bool Connection::parseLoginAnswer(const QJsonDocument &jsonDocument) {
    if (jsonDocument.isNull())
        return false;

    const auto& object = jsonDocument.object();
    const auto& data = object["data"];
    if (data.isUndefined()) {
        return false;
    }

    const auto& data_object = data.toObject();

    auto success = object["success"].toBool();
    parameters_.sid = data_object["sid"].toString();

    return success;
}

void Connection::contentOfDirectory(IdType id, std::function<void (FileListPtr)> handler) {

    QString url = QString("https://%1.fr.quickconnect.to/webapi/entry.cgi?api=SYNO.FotoTeam.Browse.Folder&version=2&method=list&offset=0&limit=100&id=%2&additional=[%22thumbnail%22]&_sid=%3").arg(parameters_.quickconnect).arg(id).arg(parameters_.sid);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, id, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(nullptr);
        } else {
            auto response = parseDirectoriesAnswer(QJsonDocument::fromJson(responseArray));
            response->setId(id);

            handler(response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

FileListPtr Connection::parseDirectoriesAnswer(const QJsonDocument &jsonDocument) {
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

        if (!dir->name().isEmpty()) {
            files->append(dir);
        }
    }

    return std::move(files);
}

void Connection::contentOfDirectoryItems(IdType id, unsigned int start_point, std::function<void (FileListPtr)> handler) {

    QString parsedParameters;

    QString url = QString("https://%1.fr.quickconnect.to/webapi/entry.cgi?api=SYNO.FotoTeam.Browse.Item&version=1&method=list&offset=%2&limit=100&folder_id=%3&additional=[%22thumbnail%22]&_sid=%4").arg(parameters_.quickconnect).arg(start_point).arg(id).arg(parameters_.sid);
    QUrl searchUrl(url);

    qDebug() << url;

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, id, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(nullptr);
        } else {
            auto response = parseDirectoryItemsAnswer(QJsonDocument::fromJson(responseArray));
            response->setId(id);

            handler(response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

FileListPtr Connection::parseDirectoryItemsAnswer(const QJsonDocument &jsonDocument) {
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

void Connection::getThumbnail(Image::Size size, IdType id, QString cacheKey, QString type, std::function<void (const QImage&)> handler) {
    QString size_text;

    switch (size) {
        case Image::Size::ORIGINAL: size_text = "original"; break;
        case Image::Size::SM: size_text = "sm"; break;
        case Image::Size::M: size_text = "m"; break;
        case Image::Size::XL: size_text = "xl"; break;
    }

    QString url = QString("https://%1.fr.quickconnect.to/webapi/entry.cgi?api=SYNO.FotoTeam.Thumbnail&method=get&version=2&id=%2&cache_key=%3&type=%4&size=%5&_sid=%6").arg(parameters_.quickconnect).arg(id).arg(cacheKey).arg(type).arg(size_text).arg(parameters_.sid);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        QImage pixmap;

        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(pixmap);
        } else {
            pixmap.loadFromData(responseArray);
            handler(pixmap);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

void Connection::countOfDirectoryItems(IdType id, std::function<void (int, int)> handler) {

    QString parsedParameters;

    QString url = QString("https://%1.fr.quickconnect.to/webapi/entry.cgi?api=SYNO.FotoTeam.Browse.Item&version=1&method=count&folder_id=%2&additional=[%22thumbnail%22]&_sid=%3").arg(parameters_.quickconnect).arg(id).arg(parameters_.sid);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, id, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(0, 0);
        } else {
            auto response = parseCountDirectoryItemsAnswer(QJsonDocument::fromJson(responseArray));
            handler(id, response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

int Connection::parseCountDirectoryItemsAnswer(const QJsonDocument &jsonDocument) {
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


void Connection::countOfDirectory(IdType id, std::function<void (int, int)> handler) {

    QString parsedParameters;

    QString url = QString("https://%1.fr.quickconnect.to/webapi/entry.cgi?api=SYNO.FotoTeam.Browse.Folder&version=2&method=count&offset=0&id=%2&additional=[%22thumbnail%22]&_sid=%3").arg(parameters_.quickconnect).arg(id).arg(parameters_.sid);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");

    auto referer = QString("https://%1.quickconnect.to").arg(parameters_.quickconnect);
    requestRaw->addHeader("Referer", referer.toUtf8());

    QObject::connect(requestRaw, &Request::finished, [this, id, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(0, 0);
        } else {
            auto response = parseCountDirectoryItemsAnswer(QJsonDocument::fromJson(responseArray));
            handler(id, response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}
