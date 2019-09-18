#pragma once

#include <vector>

#include "vec3.h"
#include "ray.h"

class material;

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    material* mat_ptr;
};

class hitable
{
public:
    virtual ~hitable() {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};


class hitable_list : public hitable
{
public:
    hitable_list() {}
    hitable_list(std::initializer_list<hitable*> l) : list(l) {}
    hitable_list(size_t n) : list(n) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    std::vector<hitable*> list;
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (auto i : list) {
        if (i != nullptr && i->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}
