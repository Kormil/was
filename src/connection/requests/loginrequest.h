#ifndef REQUEST_H
#define REQUEST_H

#include <memory>

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Connection;

template <class RequestType>
class Request : public QObject
{
    using RequestTypePtr = std::unique_ptr<RequestType>;

    Q_OBJECT
public:
    enum Status
    {
        SUCCESS,
        ERROR
    };

    Request(const QUrl& url, Connection *connection);
    void run();

    void addHeader(const QByteArray& key, const QByteArray& value);
    QList<QPair<QByteArray, QByteArray> > &getResponseHeaders();

    ~Request() {
        networkReply->deleteLater();
    }

    int serial() const;
    void setSerial(int serial);

    void close();

private slots:
    void timeout();
    void responseFinished(QNetworkReply::NetworkError error, QString errorString);

private:
    QNetworkRequest network_request_;
    QTimer request_timer_;
    Connection *connection_;
    QNetworkReply* network_reply_;
    QByteArray response_array_;
    QList<QPair<QByteArray, QByteArray>> response_headers_;
    int serial_;

    std::function<RequestType(const QJsonDocument &jsonDocument)> parser_;
    std::function<void(RequestTypePtr)> handler_;

signals:
    void finished(Status, const QByteArray&);
};

using PatameterList = std::map<QString, QString>;
using RequestPtr = std::unique_ptr<Request>; 

#endif //REQUEST_H
