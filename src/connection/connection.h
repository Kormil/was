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

    [[nodiscard]] int addRequest(std::shared_ptr<Request> request);
    bool runRequest(int serial);

    void deleteRequest(int serial);

    QNetworkAccessManager* networkAccessManager();

    void setQuickconnect(const QString& quickconnect);
    void setSid(const QString& sid);
    const ConnectionParameters& parameters() const;

    int nextSerial();

private:
    ConnectionParameters parameters_;

    std::unique_ptr<QNetworkAccessManager> m_networkAccessManager;

    std::map<int, RequestPtr> network_requests_;
    std::atomic<int> m_serial = 0;

    std::mutex m_networkRequestsMutex;
};

using ConnectionPtr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
