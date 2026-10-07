#include <chrono>
#include <iostream>

#include "lumen/camera.hpp"
#include "lumen/image.hpp"
#include "lumen/ray.hpp"
#include "lumen/vec3.hpp"
#include "lumen/version.hpp"

using namespace lumen;

// Sky: blend white to blue based on how far up the ray points.
Color ray_color(const Ray& r) {
    const Vec3 dir = unit_vector(r.direction());
    const double a = 0.5 * (dir.y + 1.0);  // map y from [-1, 1] to [0, 1]
    return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}

int main() {
    std::cout << "Lumen v" << kVersion << '\n';

    CameraConfig config;
    config.image_width = 800;
    config.vfov = 90.0;
    config.look_from = {0, 0, 0};
    config.look_at = {0, 0, -1};

    const Camera camera(config);
    Image image(camera.image_width(), camera.image_height());

    const auto start = std::chrono::steady_clock::now();

    for (int j = 0; j < image.height(); ++j) {
        for (int i = 0; i < image.width(); ++i) {
            image.set(i, j, ray_color(camera.get_ray(i, j)));
        }
    }

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