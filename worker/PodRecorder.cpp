//
// Created by 唐仁初 on 2022/9/1.
//

#include "PodRecorder.h"

namespace worker {

    PodRecorder::PodRecorder(std::string work_dir, RPCInterface *rpc_service) : work_dir_(std::move(work_dir)),
                                                                                rpc_(rpc_service) {}

    void PodRecorder::checkPodRecord(const std::string &manager, std::list<PodExitInformation> &exits) {
        for (auto record = records.begin(); record != records.end();) {
            if (record->owner() == manager) {
                ::ForkEcho echo;
                rpc_->fork({}, &*record, &echo, {});
                if (echo.fail())
                    exits.emplace_back(PodExitInformation(record->alias(), record->owner(),
                                                          "/Users/tangrenchu/CLionProjects/TinySwarm/Services/WorkerExit",
                                                          0));
                // FIXME: 这个文件不一定存在，要在启动的时候创建
                records.erase(record);
            } else {
                record++;
            }
        }
    }

    void PodRecorder::addPodRecord(const ::ForkInput *request) {
        std::filesystem::path path = work_dir_;
        path.append(request->alias());

        std::ofstream os(path);

        request->SerializeToOstream(&os);
    }

    void PodRecorder::removePodRecord(const std::string &alias) {
        std::filesystem::path path = work_dir_;
        path.append(alias);

        std::filesystem::remove(path);
    }

    void PodRecorder::readPodRecord() {
        std::filesystem::path path = work_dir_;

        for (auto &file: std::filesystem::directory_iterator(path)) {
            std::ifstream is(file);
            ForkInput input;
            input.ParseFromIstream(&is);
            records.emplace_back(input);
        }
    }
}