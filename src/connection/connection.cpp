#include "connection.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkDiskCache>
#include <QStandardPaths>

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

    QString url = "";//QString("https://%1.fr.quickconnect.to/webapi/auth.cgi?api=SYNO.API.Auth&version=3&method=login&account=%2&passwd=%3&session=FileStation&format=cookie").arg(quickconnect).arg(login).arg(password);
    QUrl searchUrl(url);

    Request* requestRaw = request(searchUrl);
    requestRaw->addHeader("Accept", "application/json");
    requestRaw->addHeader("Connection", "keep-alive");
    requestRaw->addHeader("Sec-Fetch-Dest", "document");
    requestRaw->addHeader("Sec-Fetch-Mode", "navigate");
    requestRaw->addHeader("Sec-Fetch-Site", "same-site");
    requestRaw->addHeader("Referer", "https://bartekseliga.quickconnect.to");

    QObject::connect(requestRaw, &Request::finished, [this, requestRaw, handler](Request::Status status, const QByteArray& responseArray) {
        if (status == Request::ERROR) {
            std::cout << "CONNECTION ERROR" << std::endl;
            handler(false);
        } else {
            qDebug() << responseArray;
            auto response = parseLoginAnswer(QJsonDocument::fromJson(responseArray));
            handler(response);
        }

        deleteRequest(requestRaw->serial());
    });
    requestRaw->run();
}

bool Connection::parseLoginAnswer(const QJsonDocument &jsonDocument) {
    if (jsonDocument.isNull())
        return false;

    const auto& object = jsonDocument.object();
//    const auto& data = object["data"];
//    if (data.isUndefined()) {
//        return false;
//    }

//    const auto data_object = data.toObject();
    auto success = object["success"].toBool();
    return success;
}
