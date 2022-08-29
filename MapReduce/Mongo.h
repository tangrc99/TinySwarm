//
// Created by 唐仁初 on 2022/6/12.
//

#ifndef MONGO_MONGO_H
#define MONGO_MONGO_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>

using namespace mongocxx;
using namespace bsoncxx::builder::basic;

class Mongo {
public:

    explicit Mongo(const bsoncxx::string::view_or_value &uri = "mongodb://127.0.0.1:27017"/*"mongodb://localhost:27017"*/)
            : conn(mongocxx::uri(uri)) {
        //auto session = conn.start_session();
    }

    ~Mongo() = default;

    // 应该按照 DataBase 来隔离业务，即一个 DataBase 对应一种具体服务
    auto getDataBase(const bsoncxx::string::view_or_value &db){
        return conn.database(db);
    }

    auto createSession(){

        options::client_session option;
        option.causal_consistency(true); // 不需要对会话中操作的进行排序

        return conn.start_session(option);
    }

private:
    mongocxx::client conn;
    mongocxx::instance instance{};      // mongodb-driver 需要进行初始化操作
};


#endif //MONGO_MONGO_H
