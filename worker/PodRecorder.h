//
// Created by 唐仁初 on 2022/9/1.
//

#ifndef TINYSWARM_PODRECORDER_H
#define TINYSWARM_PODRECORDER_H

#include "WorkerRpcInterface.pb.h"
#include "RPCInterface.h"
#include "PodDescriptor.h"
#include <list>
#include <filesystem>
#include <fstream>
#include <utility>

namespace worker {

/// Class PodRecorder is to record the startup information of process on disk.The records are used by worker node to
/// restart itself.
/// The record of service is deleted only when service is stopped by user or crushed itself or its manager is down.
    class PodRecorder {
    public:

        /// PodRecorder Constructor. Only used by Class WorkerNode.
        /// \param work_dir The work_dir of pod recorder.
        /// \param rpc_service The rpc_service running on Class WorkerNode.
        explicit PodRecorder(std::string work_dir, RPCInterface *rpc_service);

        /// If worker node crashed between manager's check, the worker node will try to restart Process that exits
        /// because of worker down. If not succeed, gives an error in exits.
        /// \param manager The manager of services.
        /// \param exits The Exit Information.
        void checkPodRecord(const std::string &manager, std::list<PodExitInformation> &exits);

        /// Write Process input to disk. Path: work_dir/ServiceRecord/alias
        /// \param request The process input.
        void addPodRecord(const ::ForkInput *request);

        /// Delete Process input in disk.
        /// \param alias The alias of process service.
        void removePodRecord(const std::string &alias);

        /// When worker node is up, check the service record.
        void readPodRecord();


    private:

        std::list<::ForkInput> records; // 重启时，用于读取崩溃前的启动信息

        const std::string work_dir_;
        RPCInterface *rpc_; // 用来回调使用
    };
}

#endif //TINYSWARM_PODRECORDER_H
