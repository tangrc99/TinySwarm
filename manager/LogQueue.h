//
// Created by 唐仁初 on 2022/6/13.
//

#ifndef MONGO_LOGQUEUE_H
#define MONGO_LOGQUEUE_H

#include <thread>
#include <atomic>
#include <string>
#include <utility>
#include <list>
#include <fstream>
#include <chrono>
#include <iostream>


// 目前是 0.5 us 一条 log ？？

struct LogInformation {
    std::string content_;

    explicit LogInformation(std::string content) : content_(std::move(content)) {}
    //explicit LogInformation(const std::string &content) : content_(content) {}

};

class LogQueue {
public:

    int append(const std::string &slice);

    void start();

    explicit LogQueue(const std::string &file);

    ~LogQueue();

    LogQueue(const LogQueue &other) = delete;

    LogQueue &operator=(const LogQueue &other) = delete;

private:
    bool flush();

    void LoopFlush();


    std::thread log_thread_;

    std::list<LogInformation> logs_;

    int count = 0;

    std::filesystem::path path_;
    std::ofstream log_file;

    std::atomic_flag swp_flag = ATOMIC_FLAG_INIT;
    std::atomic_flag flush_flag = ATOMIC_FLAG_INIT;

    bool quit;
    bool finished = false;

};


#endif //MONGO_LOGQUEUE_H
