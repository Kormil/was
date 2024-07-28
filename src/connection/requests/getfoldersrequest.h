#ifndef GETFOLDERSREQUEST_H
#define GETFOLDERSREQUEST_H

#include <functional>

#include "src/types/filelist.h"
#include "src/connection/request.h"

class GetFoldersRequest : public Request {
public:
    using HandlerType =  std::function<void (FileListPtr)>;

    GetFoldersRequest(IdType id, Space space, HandlerType handler);
    FileListPtr parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //GETFOLDERSREQUEST_H
