#ifndef COUNTITEMSREQUEST_H
#define COUNTITEMSREQUEST_H

#include <functional>

#include "src/connection/request.h"
#include "src/types/file.h"

class CountItemsRequest : public Request {
public:
    using HandlerType = std::function<void (int, int)>;

    CountItemsRequest(IdType id, HandlerType handler);
    int parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //COUNTITEMSREQUEST_H
