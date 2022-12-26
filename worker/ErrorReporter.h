//
// Created by 唐仁初 on 2022/11/11.
//

#ifndef TINYSWARM_ERRORREPORTER_H
#define TINYSWARM_ERRORREPORTER_H

#include "Mongo.h"
#include "PodDescriptor.h"

#include <list>
#include <thread>
#include <condition_variable>

namespace tiny_swarm::worker {

    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::kvp;


    struct PodExitInformation;


    /// Class ErrorReporter derives from mongo::Mongo. The class packages a mongodb-client and provides bulk write op.
    class ErrorReporter : public mongo::Mongo {
    public:

        /// Constructor of ErrorReporter. This will create a connection to mongodb impl and set write concern to k_unacknowledged.
        /// All errors will be inserted into Database "LOG", Collection "TINY_SWARM".
        /// \param uri The mongodb uri
        explicit ErrorReporter(const bsoncxx::string::view_or_value &uri) : Mongo(uri) {

            collection_ = mongo::Mongo::getDataBase("LOG").collection("TINY_SWARM");
        }

        /// If use daemon thread, join it.
        ~ErrorReporter() {
            quit_ = true;
            if (sender_.joinable())
                sender_.join();
        }

        /// Set mongodb write concern to w:0. This setting will save insert time and may cause data lose.
        void requireNoResponse() {
            // 设置使用最低写入安全等级 w:0，防止阻塞过长时间
            auto wc = collection_.write_concern();
            wc.acknowledge_level(mongocxx::write_concern::level::k_unacknowledged);
            collection_.write_concern(wc);
        }

        /// Append PodExitInformation to mongodb immediately. This operation won't use buffer to aggregate.
        /// \param infos The PodExitInformation to be inserted to mongodb.
        /// \return [Is insert operation ok, error]
        std::tuple<bool, std::string> insertExitInformation(const std::list<PodExitInformation> &infos) try {

            std::vector<bsoncxx::document::view> docs;
            docs.reserve(infos.size());
            for (auto &info: infos) {
                auto doc = bsoncxx::builder::basic::make_document(
                        kvp("time", mongo::utils::bsonDate()),
                        kvp("pod", info.alias),
                        kvp("service", info.desp_.service),
                        kvp("pid", info.desp_.pid),
                        kvp("params", make_document(kvp("exec", make_array(info.desp_.exe_params)),
                                                    kvp("docker", make_array(info.desp_.docker_param)))),
                        kvp("signal", info.signal),
                        kvp("error", info.getErrorText()),
                        kvp("output", info.getOutFile())
                );
                docs.emplace_back(doc);
            }

            // this may throw exception
            collection_.insert_many(docs);  // 使用批量插入可能会导致一部分文档失败，目前来看这些信息是不需要保证全部插入的

            return {true, ""};
        } catch (std::exception &e) {
            // 用于防止异常抛出，导致线程退出
            return {false, e.what()};
        }

        /// Add PodExitInformation. The infos will be first added to local buffer and then bulk written to mongodb. The insert
        /// operation will be completed by a daemon thread running function ErrorReporter::reportProcessError.
        /// \param infos The PodExitInformation to be inserted to mongodb.
        void appendExitInformation(const std::list<PodExitInformation> &infos) {
            if (infos.empty())
                return;
            std::unique_lock<std::mutex> lg(info_mtx_);
            exit_infos_.insert(exit_infos_.end(), infos.begin(), infos.end());
            cv_.notify_one();
        }

    private:

        void reportProcessError() {

            while (!quit_) {

                try {

                    std::list<PodExitInformation> infos;
                    {
                        std::unique_lock<std::mutex> lg(info_mtx_);
                        if (exit_infos_.empty()) {
                            cv_.wait(lg, [this] {
                                return !exit_infos_.empty();
                            });
                        }
                        std::swap(exit_infos_, infos);
                    }

                    std::vector<bsoncxx::document::view> docs;
                    docs.reserve(infos.size());
                    for (auto &info: infos) {
                        auto doc = bsoncxx::builder::basic::make_document(
                                kvp("time", mongo::utils::bsonDate()),
                                kvp("pod", info.alias),
                                kvp("service", info.desp_.service),
                                kvp("pid", info.desp_.pid),
                                kvp("params", make_document(kvp("exec", make_array(info.desp_.exe_params)),
                                                            kvp("docker", make_array(info.desp_.docker_param)))),
                                kvp("signal", info.signal),
                                kvp("error", info.getErrorText()),
                                kvp("output", info.getOutFile())
                        );
                        docs.emplace_back(doc);
                    }

                    collection_.insert_many(docs);  // 使用批量插入可能会导致一部分文档失败，目前来看这些信息是不需要保证全部插入的

                } catch (...) {
                    // 用于防止异常抛出，导致线程退出
                }

            }
        }

    private:
        std::condition_variable cv_;
        std::mutex info_mtx_;
        std::list<PodExitInformation> exit_infos_;    // 批插入缓冲区
        mongocxx::collection collection_;

        bool quit_ = false;
        std::thread sender_;
    };
}


#endif //TINYSWARM_ERRORREPORTER_H
