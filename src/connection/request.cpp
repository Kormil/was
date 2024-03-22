#include "request.h"

#include <iostream>

#include "connection.h"

namespace {
    int REQUEST_TIMEOUT = 20000;
}

Request::Request(const QUrl &url, Connection *connection) :
    m_networkRequest(url),
    m_connection(connection)
{
    m_networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    m_requestTimer.setSingleShot(true);

    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(timeout()));
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

    QObject::connect(networkReply, &QNetworkReply::redirected, [this](const QUrl &url) {
        qDebug() << "REDIRECT: " << url;
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
    qDebug() << "RedirectionTargetAttribute: " << networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    emit finished(SUCCESS, responseArray);
}

QList<QPair<QByteArray, QByteArray>>& Request::getResponseHeaders()
{
    return responseHeaders;
}
