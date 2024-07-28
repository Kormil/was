#ifndef GETITEMSREQUEST_H
#define GETITEMSREQUEST_H

#include <functional>

#include "src/types/filelist.h"
#include "src/connection/request.h"

class GetItemsRequest : public Request {
public:
    using HandlerType =  std::function<void (FileListPtr)>;

    GetItemsRequest(IdType id, unsigned int start_point, Space space, HandlerType handler);
    FileListPtr parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //GETITEMSREQUEST_H
