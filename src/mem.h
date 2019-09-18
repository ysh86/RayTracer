#pragma once

#include <functional>
#include <thread>

#include "vec3.h"

void mem_out(float *mem, size_t stride, int nx, int ny, std::function<vec3(int, int)> pix)
{
    int ny8 = ny >> 3;

    auto f = [&](int n){
        for (int j = (ny8 * (n+1)) - 1; j >= ny8 * n; --j) {
            for (int i = 0; i < nx; ++i) {
                vec3 col = pix(i, j);
                mem[j * stride + i * 3 + 0] = col.r();
                mem[j * stride + i * 3 + 1] = col.g();
                mem[j * stride + i * 3 + 2] = col.b();
            }
        }
    };

    std::thread t7(f,7);
    std::thread t6(f,6);
    std::thread t5(f,5);
    std::thread t4(f,4);
    std::thread t3(f,3);
    std::thread t2(f,2);
    std::thread t1(f,1);
    std::thread t0(f,0);

    t7.join();
    t6.join();
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();
    t0.join();
}
