//
// Created by 唐仁初 on 2022/7/27.
//

#ifndef TINYSWARM_PODDESCRIPTOR_H
#define TINYSWARM_PODDESCRIPTOR_H

#include <unistd.h>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>


struct PodDescriptor {
    pid_t pid;
    int type;   // 0 - host , 1 - docker
    std::string service;    // 服务名
    std::string alias; // 用于唯一区分服务的名称
    std::string exec;   // exe 文件 或者 docker 容器名
    std::string manager;   // 创建该进程的manger
    std::string out_file; // 用于输出错误的文件
    std::vector<char *> exe_params;
    std::vector<char *> docker_param;
    int restart;

    PodDescriptor(pid_t pid_, int type_, std::string service_, std::string alias_, std::string exec_,
                      std::string manager_, std::string out_file_, std::vector<char *> exe_params_,
                      std::vector<char *> docker_params_,int restart_ = 1) :
            pid(pid_), type(type_), service(std::move(service_)), alias(std::move(alias_)), exec(std::move(exec_)),
            manager(std::move(manager_)), out_file(std::move(out_file_)), exe_params(std::move(exe_params_)),
            docker_param(std::move(docker_params_)),restart(restart_) {

    }

};

// FIXME : 这里的错误信息需要服务名，而不需要 进程的 信息，并且同一种服务可能有多个实例，因此服务名要多思考咋么命名

struct ServiceExitInformation {
    std::string alias;  // 服务名称
    std::string manager;   // 创建该进程的manger
    std::string out_file;    // 进程的输出文件
    int exit_num = 0;   // 进程退出码
    int signal = 0; // 信号量
    bool core_dumped = false;

    ServiceExitInformation(std::string alias_, std::string manager_, std::string out_file_, int status)
            : alias(std::move(alias_)), manager(std::move(manager_)), out_file(std::move(out_file_)) {

        if (WIFSIGNALED(status)) {
            signal = status << 8 >> 8;
            exit_num = -1;  // -1 代表是由信号退出
        } else if (WIFEXITED(status)) {
            exit_num = status >> 8;
        }

        core_dumped = WCOREDUMP(status);
    }

    /// Parse Information and Generate Error Reason
    /// \return Error Text
    [[nodiscard]] std::string getErrorText() const {
        std::string core_dump = core_dumped ? " core dumped" : "";
        return signal > 0 ? "Interrupt by Signal " + std::to_string(signal) + core_dump :
               "Finished with exit code " + std::to_string(exit_num) + core_dump;
    }

    /// Get The Content Of The Output File.
    /// \param line Last Few lines To Get.
    /// \return File Content
    [[nodiscard]] std::string getOutFile(int line = 10) const {
        std::ifstream is(out_file, std::ios::ate);
        is.seekg(-2, is.cur);


        for (int i = 0; i < line && is.tellg() >= 0; i++) {
            while (is.peek() != is.widen('\n') && is.tellg() >= 0) {
                is.seekg(-1, is.cur);
            }
            is.seekg(-1, is.cur);
        }

        is.seekg(2, is.cur);

        return {(std::istreambuf_iterator<char>(is)),
                std::istreambuf_iterator<char>()};
    }

};


#endif //TINYSWARM_PODDESCRIPTOR_H
