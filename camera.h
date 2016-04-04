#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "vec3.h"
#include "ray.h"

class camera
{
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) { // vfov is top to bottom in degrees
        float theta = vfov * float(M_PI) / 180.0f;
        float half_height = tan(theta / 2.0f);
        float half_width = aspect * half_height;

        origin = lookfrom;

        vec3 w = vec3::unit_vector(lookfrom - lookat);
        vec3 u = vec3::unit_vector(vec3::cross(vup, w));
        vec3 v = vec3::cross(w, u);

        lower_left_corner = { -half_width, -half_height, -1.0f };
        lower_left_corner = origin - u * half_width - v * half_height - w;
        horizontal = u * 2.0f * half_width;
        vertical = v * 2.0f * half_height;
    }

    ray get_ray(float u, float v) const { return ray(origin, lower_left_corner + horizontal * u + vertical * v - origin); }

    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
};
