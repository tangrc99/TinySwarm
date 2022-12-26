//
// Created by 唐仁初 on 2022/11/10.
//

#ifndef ETCD_CLI_MANAGERECTD_H
#define ETCD_CLI_MANAGERECTD_H

#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <string>
#include <etcd/Watcher.hpp>

namespace manager {


    class ManagerECTD {
    public:
        explicit ManagerECTD(const std::string &url) : url_(url), client_(url) {

            kp_ = client_.leasekeepalive(5).get();

        }

        ~ManagerECTD() {
            kp_->Cancel();
        }

        void registry(const std::string &name, const std::string &url) {
            if (!path_in_etcd.empty()) {
                client_.rm_if(path_in_etcd, index_);
            }

            path_in_etcd = "/tinyswarm/managers/" + name;

            auto resp = client_.add(path_in_etcd, url, kp_->Lease()).get();

            if (resp.is_ok()) {
                index_ = resp.index();
            } else {
                throw std::runtime_error(resp.error_message());
            }
        }

        std::vector<std::string> getWorkersAndWatch() {
            std::vector<std::string> workers;
            auto resp = client_.ls("/tinyswarm/workers/").get();
            if (resp.is_ok()) {
                for (auto &worker: resp.values()) {
                    workers.emplace_back(worker.as_string());
                }
            }
            return workers;
        }

        void watchWorkers(const std::string &url) {
            watcher_ = std::make_shared<etcd::Watcher>(url, "/tinyswarm/workers/",
                                                       std::bind(&ManagerECTD::newWorkersCallBack, this,
                                                                 std::placeholders::_1), true);
        }

        std::vector<std::string> getNotification() {
            std::vector<std::string> res;
            std::lock_guard<std::mutex> lg(mtx);
            std::swap(res,notifies_);
            return res;
        }

        void newWorkersCallBack(const etcd::Response &resp) {
            std::lock_guard<std::mutex> lg(mtx);
            notifies_.emplace_back(resp.value().as_string());
        }

        bool prepareCreateService(const std::string &service, const std::string &url) {
            return client_.add("/tinyswarm/services/" + service, url, kp_->Lease()).get().is_ok();
        }

        bool updatePodInfo(const std::string &service, const std::string &pod, const std::string &json) {
            return client_.set("/tinyswarm/services/" + service + "/" + pod, json, kp_->Lease()).get().is_ok();
        }

        // 返回创建的 manager 地址
        std::string getServiceOwner(const std::string &service) {
            auto resp = client_.get("/tinyswarm/services/" + service).get();
            if (resp.is_ok()) {
                resp.value().as_string();
            }
            return "";
        }

    private:
        std::string path_in_etcd;   // 记录当前的根目录
        int64_t index_ = 0; // 存储在 etcd 中根目录下的 index 用来判断归属

        std::shared_ptr<etcd::KeepAlive> kp_;   // 自动续约操作
        etcd::Client client_;
        std::shared_ptr<etcd::Watcher> watcher_;
        std::mutex mtx;
        std::vector<std::string> notifies_;
        std::string url_;
    };

}
#endif //ETCD_CLI_MANAGERECTD_H
