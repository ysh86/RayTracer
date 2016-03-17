#pragma once

#include "vec3.h"
#include "ray.h"

class camera
{
public:
    camera() {}

    ray get_ray(float u, float v) const { return ray(origin, lower_left_corner + horizontal * u + vertical * v - origin); }

    vec3 lower_left_corner = { -2.0f, -1.0f, -1.0f };
    vec3 horizontal = { 4.0f, 0.0f, 0.0f };
    vec3 vertical = { 0.0f, 2.0f, 0.0f };
    vec3 origin = { 0.0f, 0.0f, 0.0f };
};
