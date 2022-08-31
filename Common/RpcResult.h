//
// Created by 唐仁初 on 2022/8/26.
//

#ifndef TINYSWARM_RPCRESULT_H
#define TINYSWARM_RPCRESULT_H

#include <string>
#include <utility>
#include <google/protobuf/message.h>

class RpcResult {
public:

    [[nodiscard]] bool isFailed() const {
        return fail;
    }

    [[nodiscard]] std::string ErrorText() const {
        return error_text;
    }

    [[nodiscard]] std::shared_ptr<google::protobuf::Message> Message() const {
        return message;
    }

    bool castMessageTo(google::protobuf::Message *msg) const {
        return msg->ParseFromString(message->SerializeAsString());
    }

    explicit RpcResult(std::string error) : fail(true), error_text(std::move(error)), message(nullptr) {}

    explicit RpcResult(std::shared_ptr<google::protobuf::Message> msg) : fail(false), message(std::move(msg)) {}


private:

    std::shared_ptr<google::protobuf::Message> message;
    bool fail;
    std::string error_text;
};


#endif //TINYSWARM_RPCRESULT_H
