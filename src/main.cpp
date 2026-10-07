#include <chrono>
#include <iostream>
#include <memory>

#include "lumen/camera.hpp"
#include "lumen/hittable.hpp"
#include "lumen/hittable_list.hpp"
#include "lumen/image.hpp"
#include "lumen/interval.hpp"
#include "lumen/material.hpp"
#include "lumen/ray.hpp"
#include "lumen/sphere.hpp"
#include "lumen/vec3.hpp"
#include "lumen/version.hpp"

using namespace lumen;

Color ray_color(const Ray& r, int depth, const Hittable& world) {
    // Bounce limit reached: no more light gathered.
    if (depth <= 0) return Color{0.0, 0.0, 0.0};

    HitRecord rec;
    // t_min = 0.001 stops a bounced ray re-hitting the surface it just left ("shadow acne").
    if (world.hit(r, Interval(0.001, kInfinity), rec)) {
        Ray scattered;
        Color attenuation;
        // The material decides where the light goes next and how much survives.
        if (rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, depth - 1, world);
        }
        return Color{0.0, 0.0, 0.0};  // absorbed
    }

    // Sky: blend white to blue based on how far up the ray points.
    const Vec3 dir = unit_vector(r.direction());
    const double a = 0.5 * (dir.y + 1.0);
    return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}

int main() {
    std::cout << "Lumen v" << kVersion << '\n';

    // Materials
    auto ground = std::make_shared<Lambertian>(Color{0.8, 0.8, 0.0});  // yellow-green matte
    auto center = std::make_shared<Lambertian>(Color{0.1, 0.2, 0.5});  // blue matte
    auto glass = std::make_shared<Dielectric>(1.5);                     // glass
    auto bubble = std::make_shared<Dielectric>(1.0 / 1.5);              // air pocket inside glass
    auto gold = std::make_shared<Metal>(Color{0.8, 0.6, 0.2}, 0.3);     // fuzzy gold
    
    // Scene
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0, ground));
    world.add(std::make_shared<Sphere>(Point3{0.0, 0.0, -1.2}, 0.5, center));
    world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.5, glass));   // outer glass
    world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.4, bubble));  // hollow inside
    world.add(std::make_shared<Sphere>(Point3{1.0, 0.0, -1.0}, 0.5, gold));

    // Camera
    CameraConfig config;
    config.image_width = 800;
    config.vfov = 45.0;
    config.samples_per_pixel = 100;
    config.max_depth = 50;
    config.look_from = {0, 0.5, 2};
    config.look_at = {0, 0, -1};

    const Camera camera(config);
    Image image(camera.image_width(), camera.image_height());

    // Render
    const auto start = std::chrono::steady_clock::now();

    for (int j = 0; j < image.height(); ++j) {
        std::clog << "\rScanlines remaining: " << (image.height() - j) << ' ' << std::flush;
        for (int i = 0; i < image.width(); ++i) {
            Color pixel_color{0.0, 0.0, 0.0};
            for (int s = 0; s < camera.samples_per_pixel(); ++s) {
                pixel_color += ray_color(camera.get_ray(i, j), camera.max_depth(), world);
            }
            image.set(i, j, pixel_color / camera.samples_per_pixel());
        }
    }
    std::clog << "\rDone.                    \n";

    const auto end = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    const char* out = "output.png";
    if (!image.write_png(out)) {
        std::cerr << "Failed to write " << out << '\n';
        return 1;
    }
    std::cout << "Rendered " << image.width() << "x" << image.height() << " in " << ms
              << " ms -> " << out << '\n';
    return 0;
}