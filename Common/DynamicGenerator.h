//
// Created by 唐仁初 on 2022/3/13.
//

#ifndef RPCSERVER_DYNAMICGENERATOR_H
#define RPCSERVER_DYNAMICGENERATOR_H

#include <filesystem>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <iostream>
#include <unistd.h>
#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include <memory>

using google::protobuf::DescriptorPool;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::compiler::DiskSourceTree;
using google::protobuf::compiler::Importer;

namespace pf = google::protobuf;
namespace fs = std::filesystem;

class DynamicGenerator {
private:
    const DescriptorPool *pool;
    std::unique_ptr<DynamicMessageFactory> m_factory;
    std::unique_ptr<Importer> importer;

public:

    ///
    /// \param disk_path The DynamicGenerator will import all .proto file in disk_path
    explicit DynamicGenerator(const std::string &disk_path) :
            m_factory(std::make_unique<google::protobuf::DynamicMessageFactory>()) {

        google::protobuf::compiler::DiskSourceTree source;
        source.MapPath("", disk_path);

        fs::path path(disk_path);

        std::list<std::string> files;

        for (const auto &file: fs::directory_iterator(path)) {

            if(file.path().extension() == ".proto")
                files.emplace_back(file.path().filename());
        }

        importer = std::make_unique<Importer>(&source,nullptr);

        //runtime compile foo.proto
        for(auto &file : files)
            importer->Import(file);

        pool = importer->pool();
    }

    const pf::ServiceDescriptor *getServiceDescriptor(const std::string &service){
        return pool->FindServiceByName(service);
    }

    Message *getMethodOutputProto(const pf::MethodDescriptor *method) {

        auto *input = method->input_type();
        const Message *tempo = m_factory->GetPrototype(input);

        return tempo->New();
    }

    auto getMethod(const std::string &service_name, const std::string &method_name) {

        return pool->FindMethodByName(service_name + "." + method_name);
    }


};


#endif //RPCSERVER_DYNAMICGENERATOR_H
