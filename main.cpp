#include <vector>
#include <random>
#include <float.h>

#include "ppm.h"
#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "sphere.h"
#include "camera.h"


inline vec3 random_in_unit_sphere(std::mt19937& gen, std::uniform_real_distribution<float>& distr)
{
    vec3 p;
    do {
        p = vec3(distr(gen), distr(gen), distr(gen)) * 2.0f - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0f);
    return p;
}

inline vec3 color(const ray& r, hitable* world, std::mt19937& gen, std::uniform_real_distribution<float>& distr)
{
    hit_record rec;
    if (world->hit(r, 0.0f, FLT_MAX, rec)) {
        // diffuse
        vec3 target = rec.p + rec.normal + random_in_unit_sphere(gen, distr);
        return color(ray(rec.p, target - rec.p), world, gen, distr) * 0.5f;
    }
    else {
        // Blue sky
        vec3 unit_direction = vec3::unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) + vec3(0.5f, 0.7f, 1.0f) * t;
    }
}

int main()
{
    const int nx = 128;
    const int ny = 64;
    const int ns = 100;
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> distr(0.0f, 1.0f);

    hitable* world = new hitable_list({
        new sphere(vec3(0, 0, -1), 0.5f),
        new sphere(vec3(0, -100.5f, -1), 100),
    });
    camera cam;

    ppm_out(nx, ny, [nx, ny, ns, &gen, &distr, &world, &cam](int i, int j) {
        vec3 col(0, 0, 0);
        for (int s = 0; s < ns; ++s) {
            float u = float(i + distr(gen)) / float(nx);
            float v = float(j + distr(gen)) / float(ny);

            ray r = cam.get_ray(u, v);
            //vec3 p = r.point_at_parameter(2.0);
            col += color(r, world, gen, distr);
        }
        col /= float(ns);
        col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b())); // gamma 2
        return col;
    });

    return 0;
}
