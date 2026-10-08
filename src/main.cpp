#include <chrono>
#include <iostream>
#include <memory>

#include "lumen/camera.hpp"
#include "lumen/hittable.hpp"
#include "lumen/hittable_list.hpp"
#include "lumen/image.hpp"
#include "lumen/interval.hpp"
#include "lumen/material.hpp"
#include "lumen/quad.hpp"
#include "lumen/ray.hpp"
#include "lumen/sphere.hpp"
#include "lumen/triangle.hpp"
#include "lumen/vec3.hpp"
#include "lumen/version.hpp"

using namespace lumen;

Color ray_color(const Ray& r, int depth, const Hittable& world, const Color& background) {
    // Bounce limit reached: no more light gathered.
    if (depth <= 0) return Color{0.0, 0.0, 0.0};

    HitRecord rec;
    // t_min = 0.001 stops a bounced ray re-hitting the surface it just left ("shadow acne").
    if (!world.hit(r, Interval(0.001, kInfinity), rec)) return background;

    // Light the surface gives off itself (only lights are non-black).
    const Color emission = rec.mat->emitted();

    Ray scattered;
    Color attenuation;
    // Lights don't scatter: just return their glow.
    if (!rec.mat->scatter(r, rec, attenuation, scattered)) return emission;

    return emission + attenuation * ray_color(scattered, depth - 1, world, background);
}

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

// ---------- Scenes ----------

// Three spheres (glass, matte, gold) lit by a ceiling panel. Sprint 2 materials test.
void spheres_under_light(HittableList& world, CameraConfig& config, Color& background) {
    auto ground = std::make_shared<Lambertian>(Color{0.8, 0.8, 0.0});
    auto center = std::make_shared<Lambertian>(Color{0.1, 0.2, 0.5});
    auto glass = std::make_shared<Dielectric>(1.5);
    auto bubble = std::make_shared<Dielectric>(1.0 / 1.5);
    auto gold = std::make_shared<Metal>(Color{0.8, 0.6, 0.2}, 0.3);
    auto light = std::make_shared<DiffuseLight>(Color{4.0, 4.0, 4.0});

    world.add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0, ground));
    world.add(std::make_shared<Sphere>(Point3{0.0, 0.0, -1.2}, 0.5, center));
    world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.5, glass));
    world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.4, bubble));
    world.add(std::make_shared<Sphere>(Point3{1.0, 0.0, -1.0}, 0.5, gold));
    world.add(std::make_shared<Quad>(Point3{-1.5, 1.5, -2.0}, Vec3{3.0, 0.0, 0.0},
                                     Vec3{0.0, 0.0, 2.0}, light));

    config.aspect_ratio = 16.0 / 9.0;
    config.image_width = 800;
    config.vfov = 45.0;
    config.samples_per_pixel = 200;
    config.max_depth = 50;
    config.look_from = {0, 0.5, 2};
    config.look_at = {0, 0, -1};

    background = Color{0.0, 0.0, 0.0};
}

// Classic Cornell box: red/green walls, white room, ceiling light, glass + mirror spheres.
void cornell_box(HittableList& world, CameraConfig& config, Color& background) {
    auto red = std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05});
    auto white = std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73});
    auto green = std::make_shared<Lambertian>(Color{0.12, 0.45, 0.15});
    auto light = std::make_shared<DiffuseLight>(Color{15.0, 15.0, 15.0});
    auto glass = std::make_shared<Dielectric>(1.5);
    auto mirror = std::make_shared<Metal>(Color{0.8, 0.85, 0.88}, 0.0);

    // Room: 555 units on each side. Camera looks down +z, so x = 555 is on the left.
    world.add(std::make_shared<Quad>(Point3{555, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, red));    // left
    world.add(std::make_shared<Quad>(Point3{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, green));    // right
    world.add(std::make_shared<Quad>(Point3{0, 0, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white));    // floor
    world.add(std::make_shared<Quad>(Point3{555, 555, 555}, Vec3{-555, 0, 0}, Vec3{0, 0, -555}, white));  // ceiling
    world.add(std::make_shared<Quad>(Point3{0, 0, 555}, Vec3{555, 0, 0}, Vec3{0, 555, 0}, white));  // back

    // Light panel, just below the ceiling
    world.add(std::make_shared<Quad>(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, light));

    // Contents
    world.add(std::make_shared<Sphere>(Point3{190, 90, 190}, 90, glass));    // front right
    world.add(std::make_shared<Sphere>(Point3{370, 100, 370}, 100, mirror)); // back left

    config.aspect_ratio = 1.0;
    config.image_width = 600;
    config.vfov = 40.0;
    config.samples_per_pixel = 500;
    config.max_depth = 50;
    config.look_from = {278, 278, -800};
    config.look_at = {278, 278, 0};

    background = Color{0.0, 0.0, 0.0};
}
// A single triangle and a tetrahedron built from 4 triangles. Sprint 3 geometry test.
void triangles_scene(HittableList& world, CameraConfig& config, Color& background) {
    auto ground = std::make_shared<Lambertian>(Color{0.5, 0.5, 0.5});
    auto red = std::make_shared<Lambertian>(Color{0.8, 0.15, 0.1});
    auto blue = std::make_shared<Lambertian>(Color{0.2, 0.35, 0.75});

    world.add(std::make_shared<Sphere>(Point3{0, -1000, 0}, 1000, ground));

    // Single triangle (left)
    world.add(std::make_shared<Triangle>(Point3{-2.2, 0, 0}, Point3{-0.4, 0, 0},
                                         Point3{-1.3, 1.5, 0}, red));

    // Tetrahedron (right): 3 base corners + apex, 4 faces
    const Point3 b0{0.5, 0, 0.6};
    const Point3 b1{1.9, 0, 0.6};
    const Point3 b2{1.2, 0, -0.7};
    const Point3 apex{1.2, 1.4, 0.1};
    world.add(std::make_shared<Triangle>(b0, b1, apex, blue));
    world.add(std::make_shared<Triangle>(b1, b2, apex, blue));
    world.add(std::make_shared<Triangle>(b2, b0, apex, blue));
    world.add(std::make_shared<Triangle>(b0, b2, b1, blue));  // base

    config.aspect_ratio = 16.0 / 9.0;
    config.image_width = 800;
    config.vfov = 40.0;
    config.samples_per_pixel = 100;
    config.max_depth = 50;
    config.look_from = {4.5, 2.5, 4};
    config.look_at = {0.2, 0.5, 0};

    background = Color{0.7, 0.8, 1.0};  // bright sky acts as the light source
}

int main() {
    std::cout << "Lumen v" << kVersion << '\n';

    HittableList world;
    CameraConfig config;
    Color background;

    // Pick which scene to render
    //cornell_box(world, config, background);
    // spheres_under_light(world, config, background);
    triangles_scene(world, config, background);

    const Camera camera(config);
    Image image(camera.image_width(), camera.image_height());

    // Render
    const auto start = std::chrono::steady_clock::now();

    for (int j = 0; j < image.height(); ++j) {
        std::clog << "\rScanlines remaining: " << (image.height() - j) << ' ' << std::flush;
        for (int i = 0; i < image.width(); ++i) {
            Color pixel_color{0.0, 0.0, 0.0};
            for (int s = 0; s < camera.samples_per_pixel(); ++s) {
                pixel_color += ray_color(camera.get_ray(i, j), camera.max_depth(), world, background);            }
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