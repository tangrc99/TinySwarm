//
// Created by 唐仁初 on 2022/8/29.
//

#ifndef TINYSWARM_RPCINTERFACE_H
#define TINYSWARM_RPCINTERFACE_H

#include "YoloInfer.pb.h"

class RpcInterface : public YoloTest {

    void testWithRoadSeg(::google::protobuf::RpcController *controller, const ::MongoImageOidRange *request,
                         ::YoloTestResult *response, ::google::protobuf::Closure *done) override {

        std::cout << "calling test\n";

        request->min_oid();
        request->max_oid();


        response->set_accuracy(1);
        response->set_ap(1);
        response->set_iou(1);
        response->set_precision(1);
        response->set_recall(1);
    }

    void testWithoutRoadSeg(::google::protobuf::RpcController *controller, const ::MongoImageOidRange *request,
                            ::YoloTestResult *response, ::google::protobuf::Closure *done) override {
        std::cout << "calling test2\n";

        request->min_oid();
        request->max_oid();


        response->set_accuracy(1);
        response->set_ap(1);
        response->set_iou(1);
        response->set_precision(1);
        response->set_recall(1);
    }


};


#endif //TINYSWARM_RPCINTERFACE_H
