//
// Created by 唐仁初 on 2022/8/28.
//

#ifndef TINYSWARM_MAPREDUCETASK_H
#define TINYSWARM_MAPREDUCETASK_H


#include <utility>
#include "RpcClient.h"
#include "YoloInfer.pb.h"


namespace mapreduce {

    using MapFunction = std::function<std::vector<Message *>(Message *, int)>;
    using ReduceFunction = std::function<MessagePtr(std::vector<MessagePtr>)>;

    /// The MapReduceTask Created by class MapReduce
    class MapReduceTask {
    public:

        enum Status {
            prepare, distribute, running, aggregate, finished, error
        };

        /// Create a MapReduce task using ip address pool.
        /// \param input Mapreduce input
        /// \param num Split num
        /// \param rpc_method The rpc method to call
        /// \param addresses Address pool
        /// \param map Map function
        /// \param reduce Reduce function
        /// \param cli The rpc client
        explicit MapReduceTask(Message *input, int num, const std::string &rpc_method,
                               std::vector<IPAddress> addresses, MapFunction map,
                               ReduceFunction reduce, std::shared_ptr<RpcClient> cli)

                : input_(input), output_(nullptr), num_(num), rpc_method_(rpc_method), map_(std::move(map)),
                  addresses_(std::move(addresses)), reduce_(std::move(reduce)), cli_(std::move(cli)),
                  addr_status_(num, 0),
                  res_(num, nullptr), status_(prepare) {

            sessions_.reserve(num_);


            // task_ = std::thread(&MapReduceTask::run, this);

        }

        ~MapReduceTask() {
            if (task_.joinable())
                task_.join();
        }

        /// Get the status of task.
        /// \return Task status
        [[nodiscard]] Status status() const {
            return status_;
        }


        /// Get the result of task.
        /// \param block If blocking get result
        /// \return Task result
        [[nodiscard]] RpcResult getResult(bool block = true) const {
            if (!block) {
                if (status_ == Status::finished)
                    return RpcResult(output_);

                return status_ == Status::error ? RpcResult("FAILED") : RpcResult("AGAIN");
            }

            while (status_ != Status::finished && status_ != Status::error) {}

            return status_ == Status::error ? RpcResult("FAILED") : RpcResult(output_);
        }

    public:

        /// start task
        void run() {

            // 分割输入参数
            split_inputs_ = map_(input_, num_);

            // 创建并启动异步rpc
            for (int i = 0; i < num_; i++) {
                auto session = cli_->createSession(addresses_[i % addresses_.size()]);
                sessions_.emplace_back(session);
            }

            status_ = Status::distribute;

            for (int i = 0; i < num_; i++) {
                sessions_[i]->async_run(rpc_method_, split_inputs_[i]);
            }


            status_ = Status::running;

            while (received_ != num_) {

                for (int i = 0; i < num_; i++) {
                    auto r = sessions_[i]->getAsyncResponse(false);

                    if (!r.isFailed()) {
                        received_ += (res_[i] == nullptr) ? 1 : 0;
                        res_[i] = r.Message();
                        continue;
                    }

                    if (r.isFailed() && r.ErrorText() == RPC_AGAIN)
                        continue;

                    if (r.isFailed() && r.ErrorText() != RPC_AGAIN) {
                        transfer(i);
                    }
                }

                if (Status::error == status_)
                    return;
            }

            status_ = Status::aggregate;

            output_ = reduce_(res_);

            status_ = Status::finished;
        }

        //FIXME : 函数还没有写完
        /// Randomly get an available address from address pool. If there is no available address,
        /// function returns nullptr.
        /// \return The ptr of address
        IPAddress *getRandomAddress() {
            status_ = Status::error;
            return nullptr;
        }

        /// Transfer session task if failed
        /// \param session_no The No of failed session
        void transfer(int session_no) {

            // 如果已经不可能完成任务，那么则放弃调度
            if (Status::error == status_)
                return;

            // 将当前session 对应的 ip 地址设为不可用
            for (int i = 0; i < addresses_.size(); i++) {
                if (sessions_[session_no]->ipAddress() == addresses_[i])
                    addr_status_[i] = -1;
            }

            // 如果有节点已经完成任务，则调度任务给该结点
            for (auto &session: sessions_) {
                auto rr = session->getAsyncResponse(false);
                if (!rr.isFailed()) {
                    sessions_[session_no] = cli_->createSession(session->ipAddress());

                    return;
                }
            }

            // 如果没有节点已经完成任务，那么在可用地址中随机分配一个地址
            auto addr = getRandomAddress();


            if (addr == nullptr) {
                return;
            }

            sessions_[session_no] = cli_->createSessionOnNewChannel(*addr);

        }


        std::thread task_;

        Message *input_;
        MessagePtr output_;
        int num_;
        const std::string &rpc_method_;

        std::vector<IPAddress> addresses_;
        std::vector<int> addr_status_;   // -1 不可用, 0 可用

        MapFunction map_;
        ReduceFunction reduce_;


        std::vector<Message *> split_inputs_;

        std::vector<MessagePtr> res_;
        std::vector<std::shared_ptr<Session>> sessions_;
        std::shared_ptr<RpcClient> cli_;

        int received_ = 0;

        Status status_;
    };

}


#endif //TINYSWARM_MAPREDUCETASK_H
