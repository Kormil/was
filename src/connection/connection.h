#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <functional>

#include "request.h"
#include "src/types/file.h"
#include "src/types/dir.h"
#include "src/types/image.h"
#include "src/models/filelistmodel.h"

class Connection
{
    struct ConnectionParameters {
        QString quickconnect;
        QString sid;
    };

public:
    enum Errors {
        NoData = -1
    };

    Connection();
    virtual ~Connection() {}

    void login(QString quickconnect, QString login, QString password, std::function<void(bool)> handler);
    void contentOfPhotoDirectory(std::function<void (FileListPtr)> handler);
    void contentOfDirectory(IdType id, std::function<void (FileListPtr)> handler);
    void contentOfDirectoryItems(IdType id, std::function<void (FileListPtr)> handler);

    Request* request(const QUrl &requestUrl);
    void deleteRequest(int serial);
    void clearRequests();

    QNetworkAccessManager* networkAccessManager();

protected:
    int nextSerial();

private:
    bool parseLoginAnswer(const QJsonDocument &jsonDocument);
    FileListPtr parseDirectoriesAnswer(const QJsonDocument &jsonDocument);
    FileListPtr parseDirectoryItemsAnswer(const QJsonDocument &jsonDocument);

    ConnectionParameters parameters_;

    std::unique_ptr<QNetworkAccessManager> m_networkAccessManager;
    std::map<int, RequestPtr> m_networkRequests;
    std::atomic<int> m_serial;

    std::mutex m_networkRequestsMutex;
};

using ConnectionPtr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
