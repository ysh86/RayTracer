#pragma once

#include <iostream>
#include <functional>

#include "vec3.h"

void ppm_out(int nx, int ny, std::function<vec3(int, int)> pix)
{
    // ppm header
    std::cout << "P3" << std::endl;
    std::cout << nx << " " << ny << std::endl;
    std::cout << "255" << std::endl;

    // ppm pix
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 col = pix(i, j);
            int ir = int(255.99f * col.r());
            int ig = int(255.99f * col.g());
            int ib = int(255.99f * col.b());
            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
    }
}
