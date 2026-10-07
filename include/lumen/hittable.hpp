#pragma once

#include "lumen/interval.hpp"
#include "lumen/ray.hpp"
#include "lumen/vec3.hpp"

namespace lumen {

class Material;  // forward declaration: material.hpp includes this file

struct HitRecord {
    Point3 p;
    Vec3 normal;  // always points against the incoming ray
    const Material* mat = nullptr;  // non-owning: the object owns its material
    double t = 0.0;
    bool front_face = false;  // true if the ray hit the outside of the surface

    // outward_normal must be unit length.
    void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0.0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Anything a ray can hit. Every shape (sphere, triangle, BVH node) implements this.
class Hittable {
public:
    virtual ~Hittable() = default;

    // True if r hits this object with t strictly inside ray_t; fills rec with the hit.
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
};

}  // namespace lumen