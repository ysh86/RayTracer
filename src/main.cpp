#include <vector>
#include <random>
#include <float.h>

#include "mem.h"
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

inline hitable* random_scene(std::mt19937& gen, std::uniform_real_distribution<float>& distr)
{
    int n = 1 + 22 * 22 + 3;
    hitable_list* hitables = new hitable_list(n);

    int i = 0;
    hitables->list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5f, 0.5f, 0.5f), gen, distr));
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            vec3 center(a + 0.9f * distr(gen), 0.2f, b + 0.9f * distr(gen));
            if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
                float choose_mat = distr(gen);
                material* m;
                if (choose_mat < 0.8f) {
                    // diffuse
                    m = new lambertian(vec3(distr(gen)*distr(gen), distr(gen)*distr(gen), distr(gen)*distr(gen)), gen, distr);
                }
                else if (choose_mat < 0.95f) {
                    // metal
                    m = new metal(vec3(0.5f*(1.0f + distr(gen)), 0.5f*(1.0f + distr(gen)), 0.5f*(1.0f + distr(gen))), 0.5f*distr(gen), gen, distr);
                }
                else {
                    // glass
                    m = new dielectric(1.5f, gen, distr);
                }
                hitables->list[i++] = new sphere(center, 0.2f, m);
            }
        }
    }
    hitables->list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f, gen, distr));
    hitables->list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(vec3(0.4f, 0.2f, 0.1f), gen, distr));
    hitables->list[i++] = new sphere(vec3(4, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f, gen, distr));

    hitables->list.resize(i);
    return hitables;
}

int main()
{
    const int nx = 480;
    const int ny = 360;
    const int ns = 100;
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> distr(0.0f, 1.0f);

    hitable* world = random_scene(gen, distr);

    vec3 lookfrom(10, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1f;
    camera cam(lookfrom, lookat, vec3(0,1,0), 30, float(nx) / float(ny), aperture, dist_to_focus, gen, distr);

    size_t stride_f3 = nx * 3;
    float mem_f3[stride_f3 * ny];
    mem_out(mem_f3, stride_f3, nx, ny, [&](int i, int j) {
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

    float *p_f3  = mem_f3;
    ppm_out(nx, ny, [&](int i, int j) {
        return vec3{
            p_f3[j * stride_f3 + i * 3 + 0], // R
            p_f3[j * stride_f3 + i * 3 + 1], // G
            p_f3[j * stride_f3 + i * 3 + 2], // B
            };
    });

    delete world;

    return 0;
}
