//
// Created by 唐仁初 on 2022/9/5.
//

#ifndef TINYSWARM_CREATERESULT_H
#define TINYSWARM_CREATERESULT_H

#include <string>
#include <utility>

struct PodDescriptor;

class CreateResult {

public:

    [[nodiscard]] bool isFail() const {
        return fail_;
    }

    std::string reason() {
        return reason_;
    }

    PodDescriptor *podDescriptor() { return pod_ }

    explicit CreateResult(PodDescriptor *pod) : fail_(false), pod_(pod){}

    explicit CreateResult(std::string reason) : fail_(true), reason_(std::move(reason)),pod_(nullptr) {}

private:
    bool fail_;
    std::string reason_;
    PodDescriptor *pod_;
};


#endif //TINYSWARM_CREATERESULT_H
