//
// Created by 唐仁初 on 2022/4/7.
//

#ifndef GOSSIP_USERINTERFACE_H
#define GOSSIP_USERINTERFACE_H

#include "GossipNode.h"
#include "UserCommandTable.h"
#include <iostream>
#include <signal.h>

class UserInterface {
private:
    bool quit = false;

    std::thread event,watch;

    GossipNode node;

    void parseCommand(const std::string &command){
        std::string tmp;
        std::vector<std::string> params;

        std::stringstream ss(command);

        while(std::getline(ss,tmp,' ')){
            params.emplace_back(tmp);
        }

        CALL_USR_COMMAND(params);
    }

    void exit(const std::vector<std::string> &params) {
        quit = true;
        node.exitEventLoop();
    }

    void showActiveNodes(const std::vector<std::string> &params){
        node.showActiveNodes();

        pthread_kill(event.native_handle(),SIGALRM);
        //FIXME: 这里会有延迟导致 ui 显示会出现一些问题
    }

    void showSleepNodes(const std::vector<std::string> &params){
        node.showSleepNodes();
    }

    void addNode(const std::vector<std::string> &params){

    }

    void addTmpNode(const std::vector<std::string> &params){

    }

    void pull(const std::vector<std::string> &params){

    }

    // 主动要求同步数据
    void push(const std::vector<std::string> &params){
        node.pushData(std::vector<std::string>(params.begin()+1,params.end()));
    }

    void data(const std::vector<std::string> &params){

    }

    static void signal_handle(int ){
        LOG_WARN("%s","SIGNAL UP");
    }

public:

    UserInterface():node(GossipNode(IPAddress(AF_INET,7001),"../properties/GossipNodes",1)){

        event = std::thread([this](){

            signal(SIGALRM,signal_handle);
            node.EventLoop();
            std::cout<<"event loop exit\n";
        });

//        sigset_t signal_mask;
//        sigemptyset(&signal_mask);
//        sigaddset(&signal_mask,SIGALRM);
//        pthread_sigmask(SIG_BLOCK,&signal_mask, nullptr);

        ADD_COMMAND("exit",std::bind(&UserInterface::exit,this,std::placeholders::_1));
        ADD_COMMAND("shutdown",std::bind(&UserInterface::exit,this,std::placeholders::_1));
        ADD_COMMAND("show_active_nodes",std::bind(&UserInterface::showActiveNodes,this,std::placeholders::_1));
        ADD_COMMAND("show_sleep_nodes",std::bind(&UserInterface::showSleepNodes,this,std::placeholders::_1));
        ADD_COMMAND("push",std::bind(&UserInterface::push,this,std::placeholders::_1));

    }

    void run() {

        while(!quit){
            std::cout << "command >> ";
            std::string command;
            std::cin >> command;

            parseCommand(command);

            usleep(500);
        }

        if(event.joinable())
            event.join();
    }
};

#endif //GOSSIP_USERINTERFACE_H
