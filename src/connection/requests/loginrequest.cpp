#include "request.h"

#include <iostream>

#include "connection.h"

namespace {
    int REQUEST_TIMEOUT = 20000;
    int NOT_SET = -1;
}

Request::Request(const QUrl &url, Connection *connection) :
    m_networkRequest(url)
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(timeout()));

    setConnection(connection);
}

void Request::run()
{
    networkReply = m_connection->networkAccessManager()->get(m_networkRequest);
    m_requestTimer.start(REQUEST_TIMEOUT);

    QObject::connect(networkReply, &QIODevice::readyRead, [this]() {
        responseArray.append(networkReply->readAll());
    });

    QObject::connect(networkReply, &QNetworkReply::finished, [this]() {
        responseFinished(networkReply->error(), networkReply->errorString());
    });
}

void Request::addHeader(const QByteArray &key, const QByteArray &value)
{
    m_networkRequest.setRawHeader(key, value);
}

int Request::serial() const
{
    return m_serial;
}

void Request::setSerial(int serial)
{
    m_serial = serial;
}

void Request::timeout()
{
    networkReply->disconnect();
    networkReply->abort();
    responseFinished(QNetworkReply::TimeoutError, tr("Request timeout"));
}

void Request::responseFinished(QNetworkReply::NetworkError error, QString errorString)
{
    m_requestTimer.stop();

    if (error != QNetworkReply::NoError)
    {
        std::cout << "ERROR: " << error << std::endl;
        emit finished(ERROR, QByteArray());
        return ;
    }

    responseHeaders = networkReply->rawHeaderPairs();
    emit finished(SUCCESS, responseArray);
}

QList<QPair<QByteArray, QByteArray>>& Request::getResponseHeaders()
{
    return responseHeaders;
}

void Request::close() {
    networkReply->disconnect();
    networkReply->abort();
    responseFinished(QNetworkReply::TimeoutError, tr("Request closed"));

    if (connection_ && serial_ != NOT_SET) {
        connection_->deleteRequest(serial_);
    }
}

void Request::setConnection(Connection *connection) {
    connection_ = connection;

    serial_ = connection->nextSerial();
}
