#pragma once

#include "lumen/ray.hpp"
#include "lumen/vec3.hpp"

namespace lumen {

struct CameraConfig {
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    double vfov = 90.0;  // vertical field of view, degrees

    Point3 look_from{0, 0, 0};  // where the camera sits
    Point3 look_at{0, 0, -1};   // what it points at
    Vec3 vup{0, 1, 0};          // which way is "up"
};

class Camera {
public:
    explicit Camera(const CameraConfig& config);

    int image_width() const { return image_width_; }
    int image_height() const { return image_height_; }

    // Ray from the camera through the centre of pixel (i, j). (0, 0) is top-left.
    Ray get_ray(int i, int j) const;

private:
    int image_width_;
    int image_height_;

    Point3 center_;
    Point3 pixel00_;      // centre of the top-left pixel
    Vec3 pixel_delta_u_;  // step one pixel right
    Vec3 pixel_delta_v_;  // step one pixel down

    Vec3 u_, v_, w_;  // camera basis: right, up, backwards
};

}  // namespace lumen