#pragma once

#include "lumen/hittable.hpp"
#include "lumen/random.hpp"
#include "lumen/ray.hpp"
#include "lumen/vec3.hpp"

namespace lumen {

// Decides what happens to light when it hits a surface.
class Material {
public:
    virtual ~Material() = default;

    // Returns true if the ray scatters. attenuation = how much of each colour survives.
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                         Ray& scattered) const = 0;
};

// Matte: scatters light in random directions around the normal.
class Lambertian : public Material {
public:
    explicit Lambertian(const Color& albedo) : albedo_(albedo) {}

    bool scatter(const Ray& /*r_in*/, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        Vec3 direction = rec.normal + random_unit_vector();

        // Random vector almost exactly opposite the normal: direction ~ 0, which breaks later maths.
        if (direction.near_zero()) direction = rec.normal;

        scattered = Ray(rec.p, direction);
        attenuation = albedo_;
        return true;
    }

private:
    Color albedo_;
};

// Shiny: reflects like a mirror, fuzz blurs the reflection.
class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : albedo_(albedo), fuzz_(fuzz < 1.0 ? fuzz : 1.0) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        Vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + fuzz_ * random_unit_vector();

        scattered = Ray(rec.p, reflected);
        attenuation = albedo_;

        // Fuzz can push the ray below the surface: absorb it instead.
        return dot(scattered.direction(), rec.normal) > 0.0;
    }

private:
    Color albedo_;
    double fuzz_;  // 0 = perfect mirror, 1 = very blurry
};

}  // namespace lumen