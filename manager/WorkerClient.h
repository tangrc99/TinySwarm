//
// Created by 唐仁初 on 2022/8/2.
//

#ifndef TINYSWARM_WORKERCLIENT_H
#define TINYSWARM_WORKERCLIENT_H

#include "Common/RpcClient.h"
#include "IPAddress.h"
#include <memory>


#define FORK "fork"
#define SHUTDOWN "shutdown"
#define CHECK "check"


namespace tiny_swarm::manager {

    /// Class WorkerClient is used to import client proto.
    class WorkerClient final : public RpcClient {
    public:


        explicit WorkerClient() : RpcClient("WorkerService", "/Users/tangrenchu/CLionProjects/TinySwarm/worker") {

        }


    };
}

#endif //TINYSWARM_WORKERCLIENT_H
