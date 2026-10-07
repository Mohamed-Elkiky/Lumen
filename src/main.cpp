#include <chrono>
#include <iostream>
#include <memory>

#include "lumen/camera.hpp"
#include "lumen/hittable.hpp"
#include "lumen/hittable_list.hpp"
#include "lumen/image.hpp"
#include "lumen/interval.hpp"
#include "lumen/random.hpp"
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
        // Diffuse bounce: random direction biased around the normal (Lambertian).
        const Vec3 direction = rec.normal + random_unit_vector();
        // Each bounce absorbs 50% of the light (grey surface).
        return 0.5 * ray_color(Ray(rec.p, direction), depth - 1, world);
    }

    // Sky: blend white to blue based on how far up the ray points.
    const Vec3 dir = unit_vector(r.direction());
    const double a = 0.5 * (dir.y + 1.0);
    return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}

int main() {
    std::cout << "Lumen v" << kVersion << '\n';

    // Scene
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3{0.0, 0.0, -1.0}, 0.5));       // centre sphere
    world.add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0));  // ground

    // Camera
    CameraConfig config;
    config.image_width = 800;
    config.vfov = 90.0;
    config.samples_per_pixel = 100;
    config.max_depth = 50;
    config.look_from = {0, 0, 0};
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