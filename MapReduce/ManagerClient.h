//
// Created by 唐仁初 on 2022/8/31.
//

#ifndef TINYSWARM_MANAGERCLIENT_H
#define TINYSWARM_MANAGERCLIENT_H

#include "Common/RpcClient.h"
#include "AddressPool.h"

using AddressVector = AddressPool::AddressVector;

class ManagerClient : public RpcClient {
public:

    explicit ManagerClient(const std::string &service, const std::string &proto_path)
            : RpcClient(service, proto_path), pool_(std::make_unique<AddressPool>()) {}

    AddressVector getAddressVec(const std::string &token) {
        return pool_->findAddresses(token);
    }

    bool SessionCallBack() override{

    }

private:

    std::unique_ptr<AddressPool> pool_;

};


#endif //TINYSWARM_MANAGERCLIENT_H
