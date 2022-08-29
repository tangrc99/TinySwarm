//
// Created by 唐仁初 on 2021/10/22.
//

#ifndef TCPTEST_TIMER_H
#define TCPTEST_TIMER_H

#include "../logUtils/Logging.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class Timer {
private:
    int pipefd[2]{};
    bool isTiming;
    bool canceled;
    bool destroyed;
    std::mutex mtx;

public:
    Timer() :isTiming(false),canceled(false),destroyed(false){
        if (-1 == ::pipe(pipefd))
            std::cout << "pipe error\n";
    }

    ~Timer(){
        ::close(pipefd[0]);
        ::close(pipefd[1]);
        LOG_WARN("%s","Timer Destruction")
    }

    void cancel(){
        canceled = true;
    }

    void destroy(){
        destroyed = true;
    }

    [[nodiscard]] bool isCanceled() const{
        return canceled;
    }

    [[nodiscard]] bool isTimeOut() const{
        if(canceled)
            return false;
        return !isTiming;
    }

    [[nodiscard]] bool isRunning() {
        //std::lock_guard<std::mutex> gd(mtx);
        return isTiming;
    }

    void setTimeOut(const std::chrono::milliseconds &timeout_ms){
        if(isTiming){
            LOG_WARN("%s","try to reset a running timer")
            return ;
        }
        canceled = false;

        isTiming = true;

        std::thread timeout_thread([this,timeout_ms](){
            std::this_thread::sleep_for(timeout_ms);
            LOG_WARN("%s","Timer EVENT")
            //if(!destroyed){
            ::write(pipefd[1]," ",1);
            //}
            {
                //std::lock_guard<std::mutex> gd(mtx);
                isTiming = false;
            }
        });
        timeout_thread.detach();
    }

    int getNoticeFd(){
        return pipefd[0];
    }

};

typedef std::shared_ptr<Timer> TimerPtr;

class TimerQueue{
private:

    std::map<int ,std::shared_ptr<Timer>> timers;
    std::vector<int> unused_id;

public:

    auto findTimer(const int &id){
        return timers.find(id);
    }

    int removeTimer(const int &id){
        auto p = findTimer(id);
        if(p==timers.end())
            return -1;
        timers.erase(p);
        return 0;
    }

    std::pair<int,TimerPtr> getTimer(){
        int id;
        if(!unused_id.empty())
            id = unused_id.back();
        id = timers.size() ;
        auto timer = std::make_shared<Timer>();
        timers.emplace(id,timer);
        return std::make_pair(id,timer);
    }


};


#endif //TCPTEST_TIMER_H
