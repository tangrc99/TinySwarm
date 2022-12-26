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

namespace tiny_swarm::worker {

    class LiveProbe;
    class TCPProbe;

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
        int port_;
        LiveProbe *probe_;

        PodDescriptor(pid_t pid_, int type_, std::string service_, std::string alias_, std::string exec_,
                      std::string manager_, std::string out_file_, std::vector<char *> exe_params_,
                      std::vector<char *> docker_params_, int port, int restart_ = 1);

        ~PodDescriptor();

    };


    struct PodExitInformation {
        std::string alias;  // 服务名称
        std::string manager;   // 创建该进程的manger
        std::string out_file;    // 进程的输出文件
        std::string error_text; // 这里是存储 worker 可以确定的错误
        int exit_num = 0;   // 进程退出码
        int signal = 0; // 信号量
        bool core_dumped = false;

        PodDescriptor desp_;

        PodExitInformation(std::string alias_, std::string manager_, std::string out_file_, int status,
                           const PodDescriptor& desp, std::string error = "") : alias(std::move(alias_)),
                                                                         manager(std::move(manager_)),
                                                                         out_file(std::move(out_file_)), desp_(desp),
                                                                         error_text(std::move(error)) {

            if (status != -1) {
                // 这里解析由进程退出收集来的信息
                if (WIFSIGNALED(status)) {
                    signal = status << 8 >> 8;
                    exit_num = -1;  // -1 代表是由信号退出
                } else if (WIFEXITED(status)) {
                    exit_num = status >> 8;
                }
                core_dumped = WCOREDUMP(status);
            }
        }

        /// Parse Information and Generate Error Reason
        /// \return Error Text
        [[nodiscard]] std::string getErrorText() const {
            if (!error_text.empty())
                return error_text;

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
}

#endif //TINYSWARM_PODDESCRIPTOR_H
