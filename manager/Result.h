//
// Created by 唐仁初 on 2022/9/5.
//

#ifndef TINYSWARM_RESULT_H
#define TINYSWARM_RESULT_H

#include <string>
#include <utility>

namespace manager {

    struct PodDescriptor;

    class Result {
    public:

        [[nodiscard]] bool isFail() const {
            return fail_;
        }

        std::string reason() {
            return reason_;
        }

        explicit Result() : fail_(false) {}

        explicit Result(std::string reason) : fail_(true), reason_(std::move(reason)) {}

    private:
        bool fail_;
        std::string reason_;
    };


    class CreateResult {

    public:

        [[nodiscard]] bool isFail() const {
            return fail_;
        }

        std::string reason() {
            return reason_;
        }

        PodDescriptor *podDescriptor() {
            return pod_;
        }

        explicit CreateResult(PodDescriptor *pod) : fail_(false), pod_(pod) {}

        explicit CreateResult(std::string reason) : fail_(true), reason_(std::move(reason)), pod_(nullptr) {}

    private:
        bool fail_;
        std::string reason_;
        PodDescriptor *pod_;
    };
}

#endif //TINYSWARM_RESULT_H
