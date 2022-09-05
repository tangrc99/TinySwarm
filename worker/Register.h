//
// Created by 唐仁初 on 2022/9/3.
//

#ifndef TINYSWARM_REGISTER_H
#define TINYSWARM_REGISTER_H

#include <string>

/// Class Register is used to register ip of worker to registry center.
class Register {
public:

    Register() = default;

    ~Register() = default;

    virtual bool registry(const std::string &address) = 0;
};

class DefaultRegister final : public Register {
public:

    bool registry(const std::string &address) override {
        return false;
    }
};


#endif //TINYSWARM_REGISTER_H
