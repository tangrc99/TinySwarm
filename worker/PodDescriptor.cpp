//
// Created by 唐仁初 on 2022/9/13.
//

#include "PodDescriptor.h"
#include "worker/probe/LiveProbe.h"

namespace worker {


    PodDescriptor::PodDescriptor(pid_t pid_, int type_, std::string service_, std::string alias_, std::string exec_,
                                 std::string manager_, std::string out_file_, std::vector<char *> exe_params_,
                                 std::vector<char *> docker_params_, int port, int restart_) :
            pid(pid_), type(type_), service(std::move(service_)), alias(std::move(alias_)), exec(std::move(exec_)),
            manager(std::move(manager_)), out_file(std::move(out_file_)), exe_params(std::move(exe_params_)),
            docker_param(std::move(docker_params_)), port_(port), restart(restart_) {

        probe_ = port_ > 0 ? new TCPProbe : new LiveProbe;
    }

    PodDescriptor::~PodDescriptor() {
        delete probe_;
    }

}