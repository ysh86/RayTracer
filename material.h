#pragma once

#include <random>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"

class material
{
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;

    static inline vec3 random_in_unit_sphere(std::mt19937& gen, std::uniform_real_distribution<float>& distr)
    {
        vec3 p;
        do {
            p = vec3(distr(gen), distr(gen), distr(gen)) * 2.0f - vec3(1, 1, 1);
        } while (p.squared_length() >= 1.0f);
        return p;
    }

    static inline vec3 reflect(const vec3& v, const vec3& n) {
        return v - n * 2 * vec3::dot(v, n);
    }
};

class lambertian : public material
{
public:
    lambertian(const vec3& a, std::mt19937& g, std::uniform_real_distribution<float>& d) : albedo(a), gen(g), distr(d) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere(gen, distr);
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }

    vec3 albedo;

    std::mt19937& gen;
    std::uniform_real_distribution<float>& distr;
};

class metal : public material
{
public:
    metal(const vec3& a, float f, std::mt19937& g, std::uniform_real_distribution<float>& d) : albedo(a), gen(g), distr(d) {
        if (f < 1) {
            fuzz = f;
        }
        else {
            fuzz = 1;
        }
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(vec3::unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + random_in_unit_sphere(gen, distr) * fuzz);
        attenuation = albedo;
        return (vec3::dot(scattered.direction(), rec.normal) > 0);
    }

    vec3 albedo;
    float fuzz;

    std::mt19937& gen;
    std::uniform_real_distribution<float>& distr;
};
