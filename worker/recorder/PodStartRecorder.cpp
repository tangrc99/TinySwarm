//
// Created by 唐仁初 on 2022/9/1.
//

#include "PodStartRecorder.h"
#include <sstream>

namespace worker::recorder {

    PodStartRecorder::PodStartRecorder(std::string work_dir, RPCInterface *rpc_service) : work_dir_(std::move(work_dir)),
                                                                                rpc_(rpc_service){
        auto path = work_dir_;
        path.append("timestamp");

        std::ifstream t_is(path);
        std::stringstream ss;
        ss << t_is.rdbuf();
        ss >> timestamp;

        if( (time(nullptr) - timestamp) > 5){
            removeAllRecords();
        }

        t_os.open(path);

        if(!t_os.is_open())
            std::cerr << "unable to open t_os\n";
    }

    void PodStartRecorder::checkPodRecord(const std::string &manager, std::list<PodExitInformation> &exits) {

        for (auto record = records.begin(); record != records.end();) {
            if (record->owner() == manager) {

                ::ForkEcho echo;
                rpc_->fork({}, &*record, &echo, {});
//FIXME

//                if (echo.fail())
//                    exits.emplace_back(PodExitInformation(record->alias(), record->owner(),
//                                                          "/Users/tangrenchu/CLionProjects/TinySwarm/Services/WorkerExit",
//                                                          0));

                records.erase(record);
            } else {
                record++;
            }
        }

        updateRecordTimestamp();
    }

    void PodStartRecorder::addPodRecord(const ::ForkInput *request) {
        std::filesystem::path path = work_dir_;
        path.append(request->alias() + ".pf");

        std::ofstream os(path);

        request->SerializeToOstream(&os);

        updateRecordTimestamp();
    }

    void PodStartRecorder::removePodRecord(const std::string &alias) {
        std::filesystem::path path = work_dir_;
        path.append(alias + ".pf");

        std::filesystem::remove(path);

        updateRecordTimestamp();
    }

    void PodStartRecorder::readPodRecord() {
        for (auto &file: std::filesystem::directory_iterator(work_dir_)) {
            std::ifstream is(file);
            ForkInput input;
            input.ParseFromIstream(&is);
            records.emplace_back(input);
        }
    }

    void PodStartRecorder::updateRecordTimestamp() {
        timestamp = time(nullptr);
        t_os << timestamp;
    }

    void PodStartRecorder::removeAllRecords() {

        for(auto &file : std::filesystem::directory_iterator(work_dir_)){
            if(file.path().extension() == ".pf")
                std::filesystem::remove(file);
        }
    }

}