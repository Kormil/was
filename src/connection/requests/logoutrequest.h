#ifndef LOGOUTREQUEST_H
#define LOGOUTREQUEST_H

#include <functional>

#include "src/connection/request.h"

class LogoutRequest : public Request {
public:
    using HandlerType = std::function<void ()>;

    LogoutRequest(HandlerType handler);
};


#endif //LOGOUTREQUEST_H
