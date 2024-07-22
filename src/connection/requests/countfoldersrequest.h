#ifndef COUNTFOLDERSREQUEST_H
#define COUNTFOLDERSREQUEST_H

#include <functional>

#include "src/connection/request.h"
#include "src/types/file.h"

class CountFoldersRequest : public Request {
public:
    using HandlerType = std::function<void (int, int)>;

    CountFoldersRequest(IdType id, HandlerType handler);
    int parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //COUNTFOLDERSREQUEST_H
