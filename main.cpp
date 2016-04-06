#include <vector>
#include <random>
#include <float.h>

#include "ppm.h"
#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


inline vec3 color(const ray& r, hitable* world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001f, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        }
        else {
            return vec3(0, 0, 0);
        }
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
    const int nx = 128;// *10;
    const int ny = 64;// *10;
    const int ns = 100;
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> distr(0.0f, 1.0f);

    hitable* world = new hitable_list({
        new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.1f, 0.2f, 0.5f), gen, distr)),
        new sphere(vec3(0, -100.5f, -1), 100, new lambertian(vec3(0.8f, 0.8f, 0.0f), gen, distr)),
        new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.0f, gen, distr)),
        new sphere(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f, gen, distr)),
        new sphere(vec3(-1, 0, -1), -0.45f, new dielectric(1.5f, gen, distr)),
    });

    vec3 lookfrom(3, 3, 2);
    vec3 lookat(0, 0, -1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 2.0f;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx) / float(ny), aperture, dist_to_focus, gen, distr);

    ppm_out(nx, ny, [nx, ny, ns, &gen, &distr, &world, &cam](int i, int j) {
        vec3 col(0, 0, 0);
        for (int s = 0; s < ns; ++s) {
            float u = float(i + distr(gen)) / float(nx);
            float v = float(j + distr(gen)) / float(ny);

            ray r = cam.get_ray(u, v);
            //vec3 p = r.point_at_parameter(2.0);
            col += color(r, world, 0);
        }
        col /= float(ns);
        col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b())); // gamma 2
        return col;
    });

    return 0;
}
