//
// Created by 唐仁初 on 2022/4/8.
//

#ifndef GOSSIP_USERCOMMANDTABLE_H
#define GOSSIP_USERCOMMANDTABLE_H

#include <map>
#include <string>
#include <functional>
#include <iostream>
#include <vector>

class UserCommandTable {
private:

    using UsrCommad = std::function<void(std::vector<std::string>)>;
    using CommandTable = std::map<std::string,UsrCommad>;

    using CommandTablePtr = std::shared_ptr<CommandTable>;
    CommandTablePtr command_table;

    void printEmptyCommand(const std::vector<std::string> &params){
        std::cout<< "Can't find command: "<< params[0] <<std::endl;

        std::cout << "Available Commands :\n";
        for(auto &command : *command_table){
            std::cout<<command.first<<"\n";
        }
    }

    UserCommandTable() : command_table(std::make_shared<CommandTable>()){}

public:

    static UserCommandTable CommandTableInitial(){
        static UserCommandTable table;
        return table;
    }

    void registerCommand(const std::string &name,const UsrCommad &command){
        command_table->emplace(name,command);
    }

    UsrCommad findCommandByName(const std::string &name){
        auto pos = command_table->find(name);
        if( pos == command_table->end())
            return [this](auto && PH1) { printEmptyCommand(std::forward<decltype(PH1)>(PH1)); };

        return pos->second;
    }

    void callCommand(const std::vector<std::string> &params){

        auto command = findCommandByName(params[0]);

        command(params);
    }


};

#define CALL_USR_COMMAND(params)  UserCommandTable::CommandTableInitial().callCommand(params)
#define ADD_COMMAND(name,function) UserCommandTable::CommandTableInitial().registerCommand(name,function)

#endif //GOSSIP_USERCOMMANDTABLE_H
