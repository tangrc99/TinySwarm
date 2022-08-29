//
// Created by 唐仁初 on 2022/4/7.
//

#ifndef GOSSIP_COMMONPARAMS_H
#define GOSSIP_COMMONPARAMS_H

#define MESSAGE_SPREAD_TIME 3   // 传播消息的时间间隔
#define MESSAGES_ALIVE_TIME 3*MESSAGE_SPREAD_TIME   // 一条消息在创造者的存活时间
#define ANTI_ENTROPY_TIME 300   // 发送全部数据的时间
#define NODE_ALIVE_TIME 300     // 定期清除无用连接
#define USR_COMMAND_DELAY 0     // 用户命令的延迟时间
#define RECOVER_SLEEP_NODE_TIME 600 // 定期尝试重连休眠节点


#endif //GOSSIP_COMMONPARAMS_H


