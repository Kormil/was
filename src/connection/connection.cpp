#include "connection.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <QPixmap>

#include "request.h"
#include "src/connection/requests/loginrequest.h"

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
    network_requests_.erase(serial);
}

QNetworkAccessManager* Connection::networkAccessManager()
{
    return m_networkAccessManager.get();
}

int Connection::nextSerial()
{
    return m_serial.fetch_add(1) + 1;
}

int Connection::addRequest(std::shared_ptr<Request> request)
{
    std::lock_guard<std::mutex> lock(m_networkRequestsMutex);

    QObject::connect(request.get(), &Request::done, [this](int serial) {
        qDebug() << "remove request: " << serial;
        deleteRequest(serial);
    });

    auto serial = nextSerial();
    network_requests_[serial] = request;

    request->setSerial(serial);

    return serial;
}

bool Connection::runRequest(int serial)
{
    RequestPtr request;
    {
        std::lock_guard<std::mutex> lock(m_networkRequestsMutex);

        auto request_it = network_requests_.find(serial);
        if (request_it == network_requests_.end()) {
            return false;
        }

        request = request_it->second;
    }

    request->run(this);
    return true;
}

const Connection::ConnectionParameters& Connection::parameters() const {
    return parameters_;
}

void Connection::setQuickconnect(const QString& quickconnect) {
    parameters_.quickconnect = quickconnect;
}

void Connection::setSid(const QString& sid) {
    parameters_.sid = sid;
}
