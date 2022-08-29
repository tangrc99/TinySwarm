//
// Created by 唐仁初 on 2022/4/2.
//

#ifndef GOSSIP_COMMONSTRUCT_H
#define GOSSIP_COMMONSTRUCT_H

#include "Common/IPAddress.h"
#include "src/Buffer.h"
#include <ctime>
#include <utility>
#include <list>


// describe the status of gossip node channel
struct GossipChannel {
    int fd;
    std::shared_ptr<IPAddress> addr;

    bool isPeer;

    int events = 0, revents = 0;

    time_t recent_time;

    Buffer buffer{};

    GossipChannel(int fd_, std::shared_ptr<IPAddress> addr_, bool peer = true) : fd(fd_), addr(std::move(addr_)),
                                                                                 isPeer(peer),
                                                                                 recent_time(time(nullptr)) {}

    size_t readToBuffer() {
        return buffer.readToBuffer(fd);
    }
};

using GossipPtr = std::shared_ptr<GossipChannel>;


// string with live time
struct MessagePack {
    time_t born_time;
    time_t expired_time;
    std::string message;

    explicit MessagePack(std::string message, int alive_time = 5) : born_time(time(nullptr)),
                                                                    expired_time(alive_time + born_time),
                                                                    message(std::move(message)) {
    }

    explicit MessagePack(const std::string &message, time_t born_time, int alive_time) :
            born_time(born_time), expired_time(born_time + alive_time), message(message) {}
};


class Command {
private:
    time_t expired_time;
    std::function<void()> command;
    bool canceled = false;

public:
    explicit Command(std::function<void()> functor, time_t alive_time = 30) : command(std::move(functor)) {
        expired_time = time(nullptr) + alive_time;
    }

    Command(const Command &c) = default;

    Command &operator=(const Command &c) = delete;

    Command(Command &&c) noexcept: command(std::move(c.command)), expired_time(c.expired_time) { c.canceled = true; }

    ~Command() {
        if (!canceled)
            command();
    }

    Command getCommandReplicate(time_t alive_time) {
        return std::move(Command{command, alive_time});
    }

    void cancel() {
        canceled = true;
    }

    bool operator<(const Command &c) const {
        return this->expired_time < c.expired_time;
    }

    bool operator==(const Command &c) const {
        return this->expired_time == c.expired_time;
    }

    [[nodiscard]] time_t expiredTime() const {
        return expired_time;
    }
};

struct RepeatedCommand {
public:
    Command command;
    time_t repeated_time;

    RepeatedCommand(Command command, time_t time) : command(std::move(command)), repeated_time(time) {}

    [[nodiscard]] time_t expiredTime() const {
        return command.expiredTime();
    }

    RepeatedCommand getCommandReplicate(time_t alive_time) {
        return std::move(RepeatedCommand(command.getCommandReplicate(alive_time), repeated_time));
    }

    void cancel() {
        command.cancel();
    }
};

class CommandQueue {
private:
    std::list<Command> commands;
    std::list<RepeatedCommand> re_commands;


    void runRepeatedCommand(const time_t &now) {
        for (auto command = re_commands.begin(); command != re_commands.end();) {
            if (command->expiredTime() <= now) {

                re_commands.emplace_back(std::move(command->getCommandReplicate(command->repeated_time)));
                command = re_commands.erase(command);
            } else {
                command++;
            }
        }
    }

    void runOnceCommand(const time_t &now) {
        for (auto command = commands.begin(); command != commands.end();) {
            if (command->expiredTime() <= now) {
                command = commands.erase(command);
            } else {
                command++;
            }
        }
    }

public:

    void addCommand(const std::function<void()> &functor, time_t alive_time = 30) {
        commands.emplace_back(functor, alive_time);
    }

    void addRepeatedCommand(const std::function<void()> &functor, time_t repeat_time = 30) {
        re_commands.emplace_back(Command(functor, repeat_time), repeat_time);
    }

    void runExpiredCommand() {
        time_t cur_time = time(nullptr);

        runOnceCommand(cur_time);
        runRepeatedCommand(cur_time);
    }

    [[nodiscard]] time_t getUrgentTime() const {
        time_t urgent = LONG_MAX;

        for (auto &command: commands) {
            urgent = std::min(urgent, command.expiredTime());
        }
        for (auto &command: re_commands) {
            urgent = std::min(urgent, command.expiredTime());
        }
        return urgent;
    }

    ~CommandQueue() {
        for (auto &command: commands) {
            command.cancel();
        }
        for (auto &command: re_commands) {
            command.cancel();
        }

    };

};


#endif //GOSSIP_COMMONSTRUCT_H
