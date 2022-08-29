//
// Created by 唐仁初 on 2022/8/19.
//

#ifndef TINYSWARM_MAPREDUCE_H
#define TINYSWARM_MAPREDUCE_H

#include "MongoImg.h"
#include "RpcClient.h"
#include "MapReduceTask.h"
#include <mongocxx/client.hpp>

namespace mapreduce {

    /// User MapReduce Interface
    class MapReduce {
    public:

        /// Create a MapReduce task using ip address pool.
        /// \param input Mapreduce input
        /// \param num Split num
        /// \param rpc_method The rpc method to call
        /// \param addresses Address pool
        /// \param map Map function
        /// \param reduce Reduce function
        /// \return The MapReduceTask to be created.
        MapReduceTask createMapReduceTask(Message *input, int num, const std::string &rpc_method,
                                          const std::vector<IPAddress> &addresses, const MapFunction &map,
                                          const ReduceFunction &reduce) {

            return MapReduceTask(input, num, rpc_method, addresses, map, reduce, cli);
        }


//        void run(int num) {
//
//            // rpc, manager 创建出所需要的节点数量
//            std::string service_dns;
//            // service_dns = manager.getServiceDNS();
//
//            // mongodb, 从数据库拉取数据进行分配
//            auto oid_list = mongo->getTestSetImg();
//
//
//            // 通过 manager 返回的服务名称调用私有 DNS 解析地址
//            std::list<std::shared_ptr<RpcSession>> sessions;
//
//            for (int i = 0; i < num; i++) {
//                auto session = cli->createSession(IPAddress(service_dns.c_str()));
//                sessions.emplace_back(session);
//            }
//
//            // DNS 解析出的地址利用轮询机制来实现负载均衡
//
//            // 对所有数据进行整合
//
//            // 这样做的优点在于，节点如果下线，那么manager节点会更改底层的负载均衡规则，mapreduce 服务只需要保证访问服务的次数足够即可。
//            // 传统的做法则需要在应用层进行节点地址的更换
//        }


    private:

        std::unique_ptr<MongoImg> mongo;
        std::shared_ptr<RpcClient> cli;
    };

}


#endif //TINYSWARM_MAPREDUCE_H
