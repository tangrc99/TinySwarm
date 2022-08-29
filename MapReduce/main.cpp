//
// Created by 唐仁初 on 2022/8/19.
//

#include <iostream>
#include "RpcClient.h"
#include "RpcSession.h"
#include "MapReduce.h"

#include "DynamicGenerator.h"


using namespace mapreduce;


void test(){

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
    double ne = 3e18;   // 1e19
    double o_D = -q_D * B / m_D;
    double o_H = -q_H * B / m_H;
    double o_HE = -q_HE * B / m_HE;
    double w_D = sqrt( (q_D * q_D) * ne / (e0 * m_D));
    double w_H = sqrt( (q_H * q_H) * ne / (e0 * m_H));
    double w_HE = sqrt( (q_HE * q_HE)  * ne / (e0 * m_HE));
    double e_x = -(w_D * w_D) * o_D / (( (w * w) - (o_D * o_D) ) * w) - (w_H * w_H) * o_H / (( (w *w)  - (o_H *o_H)) * w);

    double e_pingxing = 1 - (w_D * w_D) / (w * w) - (w_H * w_H) / (w * w);
    double e_chuizhi = 1 - (w_D * w_D) / ((w * w) - (o_D * o_D) ) - (w_H * w_H) / (w *w  - o_H *o_H);

    std::cout << "e_x: " <<e_x << std::endl
              << "e_pingxing: " << e_pingxing << std::endl
              << "e_chuizhi: " << e_chuizhi << std::endl;
}

int main() {

    test();

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