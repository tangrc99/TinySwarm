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

    [[nodiscard]] google::protobuf::Message *Message() const {
        return message;
    }

    explicit RpcResult(std::string error) : fail(true), error_text(std::move(error)), message(nullptr) {}

    explicit RpcResult(google::protobuf::Message *message) : fail(false), message(message) {}


private:

    google::protobuf::Message *message;
    bool fail;
    std::string error_text;
};


#endif //TINYSWARM_RPCRESULT_H
