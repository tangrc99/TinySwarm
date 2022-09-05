#! /bin/bash
    double u0 = 12.56e-7;
    double BT = 3.4;
    double e0 = 8.85e-12;
    double f = 55e6;
    double w = 188.4e6;
    double m_H = 1.6726231e-27;
    double q_H = 1.602e-19;
    double m_D = 3.3452462e-27;
    double q_D = 1.602e-19;
    double m_HE = 6.6904924e-27;
    double q_HE = 3.204e-19;


    double B0 = 2.1;
    double B = 2.1;
    double ne = 3e18;
    double o_D = -q_D * B / m_D;
    double o_H = -q_H * B / m_H;
    double o_HE = -q_HE * B / m_HE;
    double w_D = sqrt((q_D * q_D) * ne / (e0 * m_D));
    double w_H = sqrt((q_H * q_H) * ne / (e0 * m_H));
    double w_HE = sqrt((q_HE * q_HE) * ne / (e0 * m_HE));
    double e_x = -(w_D * w_D) * o_D / (((w * w) - (o_D * o_D)) * w) - (w_H * w_H) * o_H / (((w * w) - (o_H * o_H)) * w);

    double e_pingxing = 1 - (w_D * w_D) / (w * w) - (w_H * w_H) / (w * w);
    double e_chuizhi = 1 - (w_D * w_D) / ((w * w) - (o_D * o_D)) - (w_H * w_H) / (w * w - o_H * o_H);

    echo "e_x: ";
     echo e_x;
     echo "e_pingxing: "