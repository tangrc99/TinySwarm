#include <utility>

//
// Created by 唐仁初 on 2022/8/31.
//

#ifndef TINYSWARM_RESULT_H
#define TINYSWARM_RESULT_H

template<class T>
class Result {
public:
    [[nodiscard]] bool succeed() const {
        return succeed_;
    }

    [[nodiscard]] std::string ErrorText() const {
        return error_text;
    }

    [[nodiscard]] T content() const {
        return message;
    }


    Result(bool res, T msg, std::string error = {}) : succeed_(res), message(std::move(msg)), error_text(std::move(error)) {}


private:
    T message;
    bool succeed_{};
    std::string error_text{};
};


#endif //TINYSWARM_RESULT_H
