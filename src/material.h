#pragma once

#include <random>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"

class material
{
public:
    virtual ~material() {}
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

    static inline bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
        vec3 uv = vec3::unit_vector(v);
        float dt = vec3::dot(uv, n);
        float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
        if (discriminant > 0) {
            refracted = (uv - n * dt) * ni_over_nt - n * sqrt(discriminant);
            return true;
        }
        else {
            return false;
        }
    }

    static inline float schlick(float cosine, float ref_idx) {
        float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
        r0 = r0 * r0;
        return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
    }
};

class lambertian : public material
{
public:
    lambertian(const vec3& a, std::mt19937& g, std::uniform_real_distribution<float>& d) : albedo(a), gen(g), distr(d) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
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
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
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

class dielectric : public material
{
public:
    dielectric(float ri, std::mt19937& g, std::uniform_real_distribution<float>& d) : ref_idx(ri), gen(g), distr(d) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0f, 1.0f, 1.0f);
        vec3 refracted{0.0f, 0.0f, 0.0f};
        float reflect_prob;
        float cosine;
        if (vec3::dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            //cosine = ref_idx * vec3::dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = vec3::dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1.0f - ref_idx * ref_idx * (1.0f - cosine * cosine));
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -vec3::dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else {
            //scattered = ray(rec.p, reflected);
            reflect_prob = 1.0f;
        }
        if (distr(gen) < reflect_prob) {
            scattered = ray(rec.p, reflected);
        }
        else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }

    float ref_idx;

    std::mt19937& gen;
    std::uniform_real_distribution<float>& distr;
};
