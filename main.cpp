#include <vector>
#include <float.h>

#include "ppm.h"
#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "sphere.h"

inline vec3 color(const ray& r, hitable* world)
{
    hit_record rec;
    if (world->hit(r, 0.0f, FLT_MAX, rec)) {
        // Surface normal
        return vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1) * 0.5f;
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

    std::vector<hitable*> list = {
        new sphere(vec3(0, 0, -1), 0.5f),
        new sphere(vec3(0, -100.5f, -1), 100),
    };
    hitable* world = new hitable_list(std::move(list));

    ppm_out(nx, ny, [nx, ny, world](int i, int j) {
        const vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
        const vec3 horizontal(4.0f, 0.0f, 0.0f);
        const vec3 vertical(0.0f, 2.0f, 0.0f);
        const vec3 origin(0.0f, 0.0f, 0.0f);

        float u = float(i) / float(nx);
        float v = float(j) / float(ny);
        ray r(origin, lower_left_corner + horizontal * u + vertical * v);
        
        //vec3 p = r.point_at_parameter(2.0);
        return color(r, world);
    });

    return 0;
}
