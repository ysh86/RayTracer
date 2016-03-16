#include "ppm.h"
#include "vec3.h"
#include "ray.h"

inline float hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = vec3::dot(r.direction(), r.direction());
    float b = vec3::dot(oc, r.direction()) * 2.0f;
    float c = vec3::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        return -1.0f;
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }
}

inline vec3 color(const ray& r)
{
    float t = hit_sphere(vec3(0, 0, -1), 0.5f, r);

    if (t > 0.0f) {
        // Surface normal
        vec3 N = vec3::unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1));
        return vec3(N.x() + 1, N.y() + 1, N.z() + 1) * 0.5f;
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

    ppm_out(nx, ny, [nx, ny](int i, int j) {
        const vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
        const vec3 horizontal(4.0f, 0.0f, 0.0f);
        const vec3 vertical(0.0f, 2.0f, 0.0f);
        const vec3 origin(0.0f, 0.0f, 0.0f);

        float u = float(i) / float(nx);
        float v = float(j) / float(ny);
        ray r(origin, lower_left_corner + horizontal * u + vertical * v);
        
        return color(r);
    });

    return 0;
}
