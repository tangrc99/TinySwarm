//
// Created by 唐仁初 on 2022/8/31.
//

#ifndef TINYSWARM_ADDRESSPOOL_H
#define TINYSWARM_ADDRESSPOOL_H

#include "IPAddress.h"
#include <map>
#include <vector>

class AddressPool {
public:

    using AddressVector = std::vector<IPAddress>;

    void addAddresses(const std::string &token, const std::vector<IPAddress> &addresses) {
        pool_.emplace(token, addresses);
    }

    AddressVector findAddresses(const std::string &token) {
        auto it = pool_.find(token);
        return it == pool_.end() ? AddressVector{} : it->second;
    }


private:

    std::map<std::string, AddressVector> pool_;
};


#endif //TINYSWARM_ADDRESSPOOL_H
