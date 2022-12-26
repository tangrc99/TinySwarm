//
// Created by 唐仁初 on 2022/6/12.
//

#ifndef MONGO_MONGO_H
#define MONGO_MONGO_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/exception/exception.hpp>

namespace mongo {

    /// Class Mongo is a abstract mongodb client.
    class Mongo {
    public:

        /// Constructor of Mongo.
        /// \param uri MongoDB uri
        explicit Mongo(const bsoncxx::string::view_or_value &uri = "mongodb://127.0.0.1:27017")
                : conn(mongocxx::uri(uri)) {

            (void) instance;    // 用来消除警告
        }

        ~Mongo() = default;

    protected:
        /// Get a database impl of connected mongodb.
        /// \param db The name of database
        /// \return Database impl
        mongocxx::database getDataBase(const bsoncxx::string::view_or_value &db) {
            return conn.database(db);
        }

        /// Create a client session of this connection.
        /// \return Client session impl
        mongocxx::client_session createSession() {

            mongocxx::options::client_session option;
            option.causal_consistency(true); // 不需要对会话中操作的进行排序

            return conn.start_session(option);
        }

    private:
        mongocxx::client conn;  // 该对象已经内置了 10 个 tcp 连接
        mongocxx::instance instance{};      // mongodb-driver 需要进行初始化操作
    };


    namespace utils {


        static bsoncxx::types::b_date bsonDate(int year, int month, int day) {
            std::tm t{};
            t.tm_year = year - 1900;
            t.tm_mon = month - 1;
            t.tm_mday = day;
            return bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(std::mktime(&t)));
        }

        static bsoncxx::types::b_date bsonDate() {
            return bsoncxx::types::b_date(std::chrono::system_clock::now());
        }

        template<typename T>
        static bsoncxx::types::bson_value::view bsonObjectToView(T value) {
            auto view = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("id", value)).view()["id"].get_value();
            return view;
        }

        static bsoncxx::types::bson_value::view toOid(const std::string &oid){
            auto view = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("id", bsoncxx::oid(oid))).view()["id"].get_value();
            return view;
        }


    }

    namespace stages {

        using bsoncxx::builder::basic::make_document;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::kvp;

        static mongocxx::options::aggregate hint(const std::string &index, int type) {
            mongocxx::options::aggregate option;
            mongocxx::hint hint(make_document(kvp(index, type)));
            option.hint(hint);
            return option;
        }

        /// project {\n  'out_filed_name' :\n
        /// { '$concat' : [ ".", \n { '$last' : { '$split' : [ '$img_name',"."]\n } } ] } }
        /// \param out_field_name Filed name to output
        /// \return Pipeline to process
        static mongocxx::pipeline getFileExtension(const std::string &out_field_name) {
            mongocxx::pipeline stages;
            stages.project(make_document(
                    kvp(out_field_name, make_document(
                            kvp("$concat", make_array(".", make_document(
                                    kvp("$last", make_document(kvp("$split", make_array("$img_name", ".")
                                    ))))))))));
            return stages;
        }




    }


}


#endif //MONGO_MONGO_H
