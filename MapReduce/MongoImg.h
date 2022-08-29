//
// Created by 唐仁初 on 2022/7/14.
//

#ifndef MONGO_MONGOIMG_H
#define MONGO_MONGOIMG_H

#include "Mongo.h"
#include <fstream>
#include <sstream>
#include <list>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

using OidBucketPair = std::pair<bsoncxx::types::b_oid,bsoncxx::types::b_oid>;

class MongoImg : public Mongo {
public:
    explicit MongoImg(const std::string &db_name)
            : bucket_(getDataBase(db_name).gridfs_bucket()),
              collection_(getDataBase(db_name).collection("ImgInfo")) {

    }

    bsoncxx::types::b_oid uploadImgFromDisk(const std::string &upload_name, const std::string &path) {
        std::ifstream is(path);
        auto res = bucket_.upload_from_stream(upload_name, &is);
        return res.id().get_oid();
    }

    bsoncxx::types::bson_value::view uploadImgFromString(const std::string &upload_name, const std::string &str) {
        std::stringstream ss(str);
        auto res = bucket_.upload_from_stream(upload_name, &ss);
        return res.id();
    }

    void downloadImgById(const bsoncxx::types::bson_value::view &id, std::ostream &os) {
        bucket_.download_to_stream(id, &os);
    }

    std::string downloadImgByIdToString(const bsoncxx::types::bson_value::view &id) {
        std::stringstream ss;
        bucket_.download_to_stream(id, &ss);
        return ss.str();
    }

    auto getImgNameByName(const std::string &name) {
        return bucket_.find(make_document(kvp("filename", name)));
    }

    std::list<bsoncxx::types::b_oid> findImgDuringPeriod(const std::string &date_1, const std::string &date_2) {

        pipeline stage;
        stage.match(make_document(
                kvp("date", make_document(
                        kvp("$gte", date_1),
                        kvp("$lte", date_2)))

        )).project(make_document(
                kvp("bucket", 1)
        ));

        auto docs = collection_.aggregate(stage);

        std::list<bsoncxx::types::b_oid> img_names;

        for (auto &doc: docs) {
            img_names.emplace_back(doc["bucket"].get_oid());
        }

        return img_names;
    }


    // FIXME : 这里需要解决上传到一半失败的问题
    auto uploadImgWithInformation(const std::string &path) {
        fs::path dir(path);

        if (!fs::is_directory(dir))
            return;

        for (auto &file: fs::directory_iterator(path)) {

            auto fn = file.path().filename();

            auto oid = uploadImgFromDisk(fn, file.path());
            auto doc = make_document(
                    kvp("img_name", fn),
                    kvp("bucket", oid),
                    kvp("date", fn.string().substr(0, 20)),
                    kvp("AGX", 3));

            collection_.insert_one(doc.view());
        }
    }


    void uploadLabelFromDisk(const std::string &img_name, const std::string &label_path) {

        pipeline stage;
        stage.match(make_document(kvp("img_name",img_name)))
        .project(make_document(kvp("_id",1))).limit(1);

        auto doc = *collection_.aggregate(stage).begin();

        uploadLabelFromDisk(doc["_id"].get_oid(),label_path);
    }

    void uploadLabelFromDisk(const bsoncxx::types::b_oid &img_id, const std::string &label_path) {

        std::ifstream label(label_path);
        std::string label_str((std::istreambuf_iterator<char>(label)),
                              std::istreambuf_iterator<char>());

        pipeline stage;
        stage.match(make_document(kvp("_id", img_id)))
        .add_fields(make_document(
                kvp("status", "labeled"),
                kvp("label",label_str)));

        auto doc = *collection_.aggregate(stage).begin();

        std::cout<< doc["_id"].get_oid().value.to_string();
    }


    // 根据时间倒叙排列，得到 N 个图像的信息，以及对应的桶信息
    auto findLatestImages(int nums = 10, int s_pos = 0) {
        pipeline stage;

        stage.sort(make_document(kvp("date", -1)))
                .project(make_document(kvp("img_name", 1), kvp("bucket", 2)))
                .skip(s_pos).limit(nums);

        auto docs = collection_.aggregate(stage);

        std::list<bsoncxx::types::b_oid> img_ids;

        for (auto &doc: docs) {
            img_ids.emplace_back(doc["bucket"].get_oid());
        }

        return img_ids;
    }

    // 必须启动副本集才能够使用mongodb的事务
    void deleteImgAndInformation(const bsoncxx::types::b_oid &img_id,const bsoncxx::types::b_oid &bucket_id){

        client_session session = createSession();

        auto rm = [this,&session, &img_id, &bucket_id](client_session *){
            collection_.delete_one(session,make_document(kvp("img_id",img_id)));
            bucket_.delete_file(session, make_document(kvp("id",bucket_id)).view()["id"].get_value());
        };

        session.with_transaction(rm);
    }

    /// Get b_oid of test images from MongoDB.
    /// \return list of b_oid
    std::list<bsoncxx::types::b_oid> getTestSetImg(){
        pipeline stage;

        stage.match(make_document(kvp("status","test")))
        .project(make_document(kvp("_id",1)));  // 找到所有 status == test 的图像，只返回 _id

        auto docs = collection_.aggregate(stage);

        std::list<bsoncxx::types::b_oid> list;

        for(auto &doc : docs){
            list.emplace_back(doc["_id"].get_oid());
        }
        return list;
    }

    /// Get b_oid, bucket, label from MongoDB. Used By Yolo Infer.
    /// \return list of image information
    auto getTestSetImgInfo(){
        pipeline stage;

        stage.match(make_document(kvp("status","test")))
                .project(make_document(kvp("_id",1),kvp("bucket",2),kvp("label",3)));

        auto docs = collection_.aggregate(stage);

        return docs;
    }

private:
    gridfs::bucket bucket_;
    collection collection_;
};


#endif //MONGO_MONGOIMG_H
