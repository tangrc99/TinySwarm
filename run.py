#! /bin/python

import math

def main():
    u0 = 12.56e-7
    BT = 3.4
    e0 = 8.85e-12
    f = 55e6
    w = 188.4e6
    m_H = 1.6726231e-27
    q_H = 1.602e-19
    m_D = 3.3452462e-27
    q_D = 1.602e-19
    m_HE = 6.6904924e-27
    q_HE = 3.204e-19

    B0 = 2.1
    B = 2.1
    ne = 3e18
    o_D = -q_D * B / m_D
    o_H = -q_H * B / m_H
    o_HE = -q_HE * B / m_HE
    w_D = math.sqrt((q_D * q_D) * ne / (e0 * m_D))
    w_H = math.sqrt((q_H * q_H) * ne / (e0 * m_H))
    w_HE = math.sqrt((q_HE * q_HE) * ne / (e0 * m_HE))
    e_x = -(w_D * w_D) * o_D / (((w * w) - (o_D * o_D)) * w) - (w_H * w_H) * o_H / (((w * w) - (o_H * o_H)) * w)

    e_pingxing = 1 - (w_D * w_D) / (w * w) - (w_H * w_H) / (w * w)
    e_chuizhi = 1 - (w_D * w_D) / ((w * w) - (o_D * o_D)) - (w_H * w_H) / (w * w - o_H * o_H)

    print("e_x: ")
    print(e_x)
    print("e_x: ")
    print(e_x)
    print("e_x: ")
    print(e_x)

