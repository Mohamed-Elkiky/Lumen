#include "lumen/camera.hpp"

#include <algorithm>
#include <cmath>

namespace lumen {

namespace {
constexpr double kPi = 3.1415926535897932385;
constexpr double degrees_to_radians(double degrees) { return degrees * kPi / 180.0; }
}  // namespace

Camera::Camera(const CameraConfig& config)
    : image_width_(config.image_width),
      image_height_(std::max(1, static_cast<int>(config.image_width / config.aspect_ratio))),
      center_(config.look_from) {
    // Viewport size from the field of view
    const double focal_length = (config.look_from - config.look_at).length();
    const double h = std::tan(degrees_to_radians(config.vfov) / 2.0);
    const double viewport_height = 2.0 * h * focal_length;
    const double viewport_width =
        viewport_height * (static_cast<double>(image_width_) / image_height_);

    // Orthonormal basis: w points backwards, u right, v up
    w_ = unit_vector(config.look_from - config.look_at);
    u_ = unit_vector(cross(config.vup, w_));
    v_ = cross(w_, u_);

    // Vectors across the viewport (v goes down because pixel rows go down)
    const Vec3 viewport_u = viewport_width * u_;
    const Vec3 viewport_v = viewport_height * -v_;

    pixel_delta_u_ = viewport_u / image_width_;
    pixel_delta_v_ = viewport_v / image_height_;

    const Point3 viewport_upper_left =
        center_ - focal_length * w_ - viewport_u / 2.0 - viewport_v / 2.0;
    pixel00_ = viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
}

Ray Camera::get_ray(int i, int j) const {
    const Point3 pixel_center = pixel00_ + i * pixel_delta_u_ + j * pixel_delta_v_;
    return {center_, pixel_center - center_};
}

}  // namespace lumen