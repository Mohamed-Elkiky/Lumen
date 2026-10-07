#pragma once

#include <memory>
#include <vector>

#include "lumen/hittable.hpp"

namespace lumen {

// A scene: a list of objects. hit() returns the closest one along the ray.
class HittableList : public Hittable {
public:
    HittableList() = default;

    void add(std::shared_ptr<Hittable> object) { objects_.push_back(std::move(object)); }
    void clear() { objects_.clear(); }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord temp;
        bool hit_anything = false;
        double closest_so_far = ray_t.max;

        for (const auto& object : objects_) {
            // Shrink the interval so only hits closer than the best so far count.
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp)) {
                hit_anything = true;
                closest_so_far = temp.t;
                rec = temp;
            }
        }
        return hit_anything;
    }

private:
    std::vector<std::shared_ptr<Hittable>> objects_;
};

}  // namespace lumen