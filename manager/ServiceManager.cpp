//
// Created by 唐仁初 on 2022/9/6.
//

#include "ServiceManager.h"

namespace manager {

    ServiceManager::Token ServiceManager::generateToken(int length) {

        char tmp;
        std::string str;

        std::random_device rd;
        std::default_random_engine random(rd());

        for (int i = 0; i < length; i++) {
            tmp = static_cast<char>(random() % 36);
            if (tmp < 10)
                tmp += '0';
            else
                tmp += ('A' - 10);
            str += tmp;
        }

        if (map_.find(str) != map_.end())
            return generateToken(length);

        return str;
    }

    void ServiceManager::startFailedPod() {

        if (start_list_.empty())
            return;

        auto serv = start_list_.front();

        int start_num = serv->restartFailedPods();

        if (start_num > 0)
            proxy_->updateAddressPool(serv->token_, serv->getAddressPool());

        if (serv->status == ServiceStatus::ready)
            start_list_.pop_front();
        else {
            start_list_.emplace_back(start_list_.front());
            start_list_.pop_front();
        }
    }

    void ServiceManager::shutdownFailedPod() {

        if (end_list_.empty())
            return;

        auto serv = end_list_.front();

        serv->stopPodsInService();

        if (serv->status == ServiceStatus::close)
            removeService(serv);
        else {
            end_list_.emplace_back(end_list_.front());
            end_list_.pop_front();
        }
    }

    void ServiceManager::removeService(ServiceDescriptor *sd) {
        map_.erase(sd->token_);

        for (auto service = start_list_.begin(); service != start_list_.end(); service++) {
            if ((*service)->token_ == sd->token_)
                start_list_.erase(service);
        }

        for (auto service = end_list_.begin(); service != end_list_.end(); service++) {
            if ((*service)->token_ == sd->token_)
                end_list_.erase(service);
        }

    }
}