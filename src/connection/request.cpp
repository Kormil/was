#include "request.h"

#include <iostream>

#include "connection.h"

namespace {
    int REQUEST_TIMEOUT = 60000;
}

Request::Request()
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void Request::setUrl(const QString& url) {
    QUrl request_url(url);
    m_networkRequest = QNetworkRequest(request_url);
    m_networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    addHeader("Accept", "application/json");
    addHeader("Connection", "keep-alive");
    addHeader("Sec-Fetch-Dest", "document");
    addHeader("Sec-Fetch-Mode", "navigate");
    addHeader("Sec-Fetch-Site", "same-site");
}

void Request::run(Connection* connection)
{
    auto quickconnect = connection->parameters().quickconnect;

    auto url = QString("https://%1.fr.quickconnect.to").arg(quickconnect);
    auto referer = QString("https://%1.quickconnect.to").arg(quickconnect);

    url.append("/webapi/");
    url.append(api_);
    if (parameters_.size()) {
        url.append("&");
        url.append(parameters_);
    }

    if (add_sid_) {
        auto sid = connection->parameters().sid;
        url.append(QString("&_sid=%1").arg(sid));
    }

    qDebug() << url;
    setUrl(url);

    addHeader("Referer", referer.toUtf8());

    networkReply = connection->networkAccessManager()->get(m_networkRequest);
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
       qDebug() << error;
        emit ready(ERROR, QByteArray());
        return ;
    }

    responseHeaders = networkReply->rawHeaderPairs();
    emit ready(SUCCESS, responseArray);
}

QList<QPair<QByteArray, QByteArray>>& Request::getResponseHeaders()
{
    return responseHeaders;
}
