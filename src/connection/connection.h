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
#include "src/types/filelist.h"

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
    void logout(std::function<void ()> handler);

    void contentOfDirectory(IdType id, std::function<void (FileListPtr)> handler);
    void contentOfDirectoryItems(IdType id, unsigned int start_point, std::function<void (FileListPtr)> handler);

    void countOfDirectory(IdType id, std::function<void (int, int)> handler);
    void countOfDirectoryItems(IdType id, std::function<void (int, int)> handler);

    Request* request(const QUrl &requestUrl);
    void deleteRequest(int serial);
    void clearRequests();

    QNetworkAccessManager* networkAccessManager();

    void getThumbnail(Image::Size size, IdType id, QString cacheKey, QString type, std::function<void (const QImage &)> handler);

protected:
    int nextSerial();

private:
    bool parseLoginAnswer(const QJsonDocument &jsonDocument);
    FileListPtr parseDirectoriesAnswer(const QJsonDocument &jsonDocument);
    FileListPtr parseDirectoryItemsAnswer(const QJsonDocument &jsonDocument);
    int parseCountDirectoryItemsAnswer(const QJsonDocument &jsonDocument);

    ConnectionParameters parameters_;

    std::unique_ptr<QNetworkAccessManager> m_networkAccessManager;
    std::map<int, RequestPtr> m_networkRequests;
    std::atomic<int> m_serial;

    std::mutex m_networkRequestsMutex;
};

using ConnectionPtr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
