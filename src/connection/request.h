#ifndef REQUEST_H
#define REQUEST_H

#include <memory>

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Connection;

class Request : public QObject
{
    Q_OBJECT
public:
    enum Status
    {
        SUCCESS,
        ERROR
    };

    Request();
    void run(Connection* connection);

    void addHeader(const QByteArray& key, const QByteArray& value);
    QList<QPair<QByteArray, QByteArray> > &getResponseHeaders();

    ~Request()
    {
        networkReply->deleteLater();
    }

    int serial() const;
    void setSerial(int serial);

protected:
    void setUrl(const QString& url);

    bool add_sid_ = true;
    QString url_;

private slots:
    void timeout();
    void responseFinished(QNetworkReply::NetworkError error, QString errorString);

private:
    QNetworkRequest m_networkRequest;
    QTimer m_requestTimer;
    QNetworkReply* networkReply;
    QByteArray responseArray;
    QList<QPair<QByteArray, QByteArray>> responseHeaders;
    int m_serial;

signals:
    void ready(Status, const QByteArray&);
    void done(int);

};

using PatameterList = std::map<QString, QString>;
using RequestPtr = std::shared_ptr<Request>;

#endif //REQUEST_H
