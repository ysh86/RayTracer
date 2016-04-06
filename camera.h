#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "vec3.h"
#include "ray.h"

class camera
{
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, std::mt19937& g, std::uniform_real_distribution<float>& d) : // vfov is top to bottom in degrees
        gen(g),
        distr(d)
    {
        lens_radius = aperture / 2.0f;

        float theta = vfov * float(M_PI) / 180.0f;
        float half_height = tan(theta / 2.0f);
        float half_width = aspect * half_height;

        origin = lookfrom;

        w = vec3::unit_vector(lookfrom - lookat);
        u = vec3::unit_vector(vec3::cross(vup, w));
        v = vec3::cross(w, u);

        lower_left_corner = origin - u * half_width * focus_dist - v * half_height * focus_dist - w * focus_dist;
        horizontal = u * 2.0f * half_width * focus_dist;
        vertical = v * 2.0f * half_height * focus_dist;
    }

    static inline vec3 random_in_unit_disk(std::mt19937& gen, std::uniform_real_distribution<float>& distr)
    {
        vec3 p;
        do {
            p = vec3(distr(gen), distr(gen), 0) * 2.0f - vec3(1, 1, 0);
        } while (vec3::dot(p,p) >= 1.0f);
        return p;
    }

    ray get_ray(float s, float t) const {
        vec3 rd = random_in_unit_disk(gen, distr) * lens_radius;
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, lower_left_corner + horizontal * s + vertical * t - origin - offset);
    }

    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u, v, w;
    float lens_radius;

    std::mt19937& gen;
    std::uniform_real_distribution<float>& distr;
};
