//
// Created by 唐仁初 on 2022/8/19.
//

#ifndef TINYSWARM_WORKERSTATUS_H
#define TINYSWARM_WORKERSTATUS_H

#include <unordered_map>
#include <string>
#include <filesystem>

class WorkerStatus {
public:

    explicit WorkerStatus(const std::string &service_dir, const std::string &file = {}) {
        if (!file.empty())
            readFile(file);

        readServices(service_dir);
    }

    void readServices(const std::string &service_dir) {
        for (auto &file: std::filesystem::directory_iterator(service_dir)) {
            if (file.is_directory() && file.path().filename() != "ServiceRecord") {
                services.emplace(file.path().filename(),"Service Information"); // FIXME: 加入服务的描述文件
            }
        }
    }

    void readFile(const std::string &file = {}) {
        if(!std::filesystem::exists(file))
            return;
    }

    std::string getStatus(const std::string &key) {
        auto it = status.find(key);
        return it != status.end() ? it->second : "";
    }

    [[nodiscard]] bool isServiceExist(const std::string &service) const{
        return services.find(service) != services.end();
    }

    // 更新状态中的参数
    void updateStatus() {
        // cpu
        // disk
        // gpu
    }

private:
    std::unordered_map<std::string, std::string> status, services;    // 用于记录各个参数

};


#endif //TINYSWARM_WORKERSTATUS_H
