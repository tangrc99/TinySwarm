//
// Created by 唐仁初 on 2022/8/19.
//

#include <iostream>
#include <nlohmann/json.hpp>

#include "RpcClient.h"
#include "MapReduce.h"
#include "ManagerRpcInterface.pb.h"
#include "DynamicGenerator.h"
#include "YoloInfer.pb.h"
#include <nlohmann/json.hpp>
#include "MongoImg.h"


using namespace mapreduce;


std::vector<Message *> map_test(Message *input, int num) {
    auto oids = pf::down_cast<MongoImageOidRange *>(input);
    return {oids};
}

MessagePtr reduce_test(const std::vector<MessagePtr> &inputs) {
    auto result = std::make_shared<YoloTestResult>();

    for (auto &input: inputs) {
        YoloTestResult res;
        MapReduce::castMessageType(input.get(), &res);
        result->set_ap(res.ap() * 2 + result->ap());
    }

    return result;
}

void mapreduce_test() {

    MapReduce mapReduce(
            std::make_shared<ManagerClient>("YoloTest", "/Users/tangrenchu/CLionProjects/TinySwarm/MapReduce"));

    MongoImageOidRange oids;
    oids.set_max_oid("100");
    oids.set_min_oid("0");

    std::vector<IPAddress> pool = {IPAddress("127.0.0.1", AF_INET, 9999)};

    auto task = mapReduce.createMapReduceTask(&oids, 1, "testWithRoadSeg", pool, map_test, reduce_test);

    task.run();

    auto res = task.getResult();

    if (res.isFailed()) {
        std::cout << res.ErrorText();
        return;
    }

    YoloTestResult result;

    result.ParseFromString(res.Message()->SerializeAsString());

    std::cout << "ap " << result.ap();
}

void yolo_test() {
    MongoImageOidRange oids;
    oids.set_max_oid("100");
    oids.set_min_oid("0");

    auto cli = std::make_shared<RpcClient>("YoloTest", "/Users/tangrenchu/CLionProjects/TinySwarm/MapReduce");

    auto session = cli->createSession(IPAddress("127.0.0.1", AF_INET, 8989));

    auto res = session->run("testWithRoadSeg", &oids);

    YoloTestResult result;
    res.castMessageTo(&result);

    std::cout << result.ap();
}

void test() {

    double u0 = 12.56e-7;
    double BT = 3.4;
    double e0 = 8.85e-12;
    double f = 55e6;
    double w = 188.4e6;  // 345.4
    double m_H = 1.6726231e-27;
    double q_H = 1.602e-19;
    double m_D = 3.3452462e-27;
    double q_D = 1.602e-19;
    double m_HE = 6.6904924e-27;
    double q_HE = 3.204e-19;


    double B0 = 2.1;
    double B = 2.1;
    double ne = 3e19;   // 1e19
    double o_D = q_D * B / m_D;
    double o_H = -q_H * B / m_H;
    double o_HE = -q_HE * B / m_HE;
    double w_D = sqrt((q_D * q_D) * ne / (e0 * m_D));
    double w_H = sqrt((q_H * q_H) * ne / (e0 * m_H));
    double w_HE = sqrt((q_HE * q_HE) * ne / (e0 * m_HE));

    double e_x = (w_D * w_D) * o_D / ((w * w - o_D * o_D) * w);

    double q_dianzi = -1.602e-19;
    double m_dianzi = 9.10956e-31;
    double w_dianzi = sqrt((q_dianzi * q_dianzi) * ne / (e0 * m_dianzi));
    double o_dianzi = q_dianzi * B / m_dianzi;
    double e_pingxing = 1.0 - (w_dianzi * w_dianzi) / (w * w);

    double R = 1.0 + (w_D * w_D) / (o_D * (w + o_D));
    double L = 1.0 - (w_D * w_D) / (o_D * (w - o_D));

    // double e_pingxing = 1.0 - (w_D * w_D) / (w * w) - (w_H * w_H) / (w * w);
    double e_chuizhi = 1.0 / (2 * (R + L));
    double e_chuizhi2 =
            1.0 - (w_D * w_D) / ((w * w) - (o_D * o_D)) - (w_dianzi * w_dianzi) / (w * w - o_dianzi * o_dianzi);

    std::cout << "w_D: " << w_D << std::endl
              << "o_D: " << o_D << std::endl
              << "e_x: " << e_x << std::endl
              << "w_dianzi: " << w_dianzi << std::endl
              << "e_pingxing: " << e_pingxing << std::endl
              << "e_chuizhi: " << e_chuizhi << std::endl
              << "e_chuizhi2: " << e_chuizhi2 << std::endl
              << "R: " << R << std::endl
              << "L: " << L << std::endl;
}

void testNew() {


    double e0 = 8.85e-12;
    double f = 55e6;
    double w = 213.628e6;  // 345.4
    double m_H = 1.6726231e-27;
    double q_H = 1.602e-19;
    double m_D = 3.3452462e-27;
    double q_D = 1.602e-19;
    double m_HE = 6.6904924e-27;
    double q_HE = 3.204e-19;

    double B0 = 2.25;
    double B = 2.25;
    double ne = 3e19;   // 1e19
    double o_D = q_D * B / m_D;
    double o_H = -q_H * B / m_H;
    double o_HE = -q_HE * B / m_HE;
    double w_D = sqrt((q_D * q_D) * ne / (e0 * m_D));
    double w_H = sqrt((q_H * q_H) * ne / (e0 * m_H));
    double w_HE = sqrt((q_HE * q_HE) * ne / (e0 * m_HE));

    double q_dianzi = -1.602e-19;
    double m_dianzi = 9.10956e-31;
    double w_dianzi = sqrt((q_dianzi * q_dianzi) * ne / (e0 * m_dianzi));

    double o_dianzi = q_dianzi * ( B / m_dianzi );

    double e_pingxing = 1.0 - (w_dianzi * w_dianzi) / (w * w);

    double e_x = (w_D * w_D) * o_D / ((w * w - o_D * o_D) * w) + (w_dianzi * w_dianzi) *o_dianzi/((w*w-o_dianzi*o_dianzi)*w);

    double R = 1.0 - (w_D * w_D) / (w * (w + o_D)) -
               (w_dianzi * w_dianzi) / (w * (w + o_dianzi));// 1.0 + (w_D * w_D) / ( o_D * (w + o_D) );
    double L = 1.0 - (w_D * w_D) / (w * (w - o_D)) -
               (w_dianzi * w_dianzi) / (w * (w - o_dianzi));//1.0 - (w_D * w_D) / ( o_D * (w - o_D) );
    double e_chuizhi = 0.5 * (R + L) ;
    double e_chuizhi2 =
            1.0 - (w_D * w_D) / ((w * w) - (o_D * o_D)) - (w_dianzi * w_dianzi) / (w * w - o_dianzi * o_dianzi);

    std::cout << "w_D: " << w_D << std::endl
              << "o_D: " << o_D << std::endl
              << "e_x: " << e_x << std::endl
              << "o_dianzi: " << o_dianzi << std::endl
              << "w_dianzi: " << w_dianzi << std::endl
              << "e_pingxing: " << e_pingxing << std::endl
              << "e_chuizhi: " << e_chuizhi << std::endl
              << "e_chuizhi2: " << e_chuizhi2 << std::endl
              << "R: " << R << std::endl
              << "L: " << L << std::endl;
}

void cast(Message *in, Message *out) {
    out->ParseFromString(in->SerializeAsString());
}


void protobuf_test() {

    DynamicGenerator generator("/Users/tangrenchu/CLionProjects/TinySwarm/MapReduce");

    auto method = generator.getServiceDescriptor("YoloTest")->FindMethodByName("testWithRoadSeg");

    MongoImageOidRange oids;
    oids.set_max_oid("100");
    oids.set_min_oid("0");

    pf::Message *m = generator.getMethodInputProto(method)->New();

    std::cout << m->GetTypeName();
    std::cout << m->ParseFromString(oids.SerializeAsString());
    std::cout << m->DebugString() << std::endl;

    // auto test = pf::down_cast<MongoImageOidRange*>(m);

    MongoImageOidRange mm;

    cast(m, &mm);

    std::cout << mm.max_oid();
}

int main() {

    ManagerClient cli("ManagerService", "/Users/tangrenchu/CLionProjects/TinySwarm/manager");

    auto session = cli.createSession(IPAddress(AF_INET, 8888));

    ServiceName name;
    name.set_token("my_redis");

    auto res = session->run("getServiceInfo", &name);

    JsonMessage jsonMessage;

    if (res.isFailed()) {
        std::cerr << res.ErrorText();
        return -1;
    }

    res.castMessageTo(&jsonMessage);

    auto json = nlohmann::json::parse(jsonMessage.content());

    std::cout << json.dump(4);

    return 0;


//    RpcClient client;
//
//    auto session = client.createSession(IPAddress(AF_INET,8989));
//
//    session->getMethodDescriptor("testWithRoadSeg");

//    auto res = dynamic_cast<YoloTestResult *>(session.getAsyncResponse());
//
//
//    std::cout << "ap" <<res->ap() << std::endl;

    return 0;
}