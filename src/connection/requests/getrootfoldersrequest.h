#ifndef GETROOTFOLDERSREQUEST_H
#define GETROOTFOLDERSREQUEST_H

#include <functional>

#include "src/types/filelist.h"
#include "src/connection/request.h"

class GetRootFoldersRequest : public Request {
public:
    using HandlerType =  std::function<void (int)>;

    GetRootFoldersRequest(Space space, HandlerType handler);
    FileListPtr parseAnswer(const QJsonDocument &jsonDocument);
};


#endif //GETROOTFOLDERSREQUEST_H
