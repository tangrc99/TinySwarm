//
// Created by 唐仁初 on 2022/8/19.
//

#ifndef TINYSWARM_WORKERSTATUS_H
#define TINYSWARM_WORKERSTATUS_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <list>

namespace worker {

    /// Class WorkerStatus is used to manage the service status and running status of WorkerNode.
    class WorkerStatus {
    public:

        /// Constructor of WorkerStatus. Only used by WorkerNode.
        /// \param service_dir The directory stored services
        /// \param file The configuration file
        explicit WorkerStatus(const std::string &service_dir, const std::string &file = {}) {
            if (!file.empty())
                readFile(file);

            readServices(service_dir);
        }

        /// Reading the available services of this worker node.
        /// @note The services should be at service_dir/service_name/service.bin
        /// \param service_dir The directory of services
        void readServices(const std::string &service_dir) {
            for (auto &file: std::filesystem::directory_iterator(service_dir)) {
                if (file.is_directory() && file.path().filename() != "ServiceRecord") {
                    services.emplace(file.path().filename()); // FIXME: 加入服务的描述文件
                }
            }
        }

        /// Reading the additional status param in file.
        /// \param file The configuration file.
        void readFile(const std::string &file = {}) {
            if (!std::filesystem::exists(file))
                return;

            std::ifstream is(file);

            std::string line;
            while (std::getline(is, line)) {
                std::stringstream ss(line);
                std::string first, second;
                ss >> first >> second;
                status.emplace(first, second);
            }
        }

        /// Get one of all status.
        /// \param key The status name
        /// \return The status info
        std::string getStatus(const std::string &key) {
            auto it = status.find(key);
            return it != status.end() ? it->second : "";
        }

        /// Check if service is available in current worker node.
        /// \param service The service name
        /// \return If service exists
        [[nodiscard]] bool isServiceExist(const std::string &service) const {
            return services.find(service) != services.end();
        }

        /// Update the changing status.
        void updateStatus() {
            // cpu
            // disk
            // gpu
        }

    private:
        std::unordered_map<std::string, std::string> status;    // 用于记录各个参数
        std::unordered_set<std::string> services;

        std::list<int> ports;

    };

}
#endif //TINYSWARM_WORKERSTATUS_H
