#include "lumen/image.hpp"

#include <algorithm>
#include <cstdint>

#include "stb_image_write.h"

namespace lumen {

namespace {
std::uint8_t to_byte(double component) {
    const double clamped = std::clamp(component, 0.0, 0.999);
    return static_cast<std::uint8_t>(256.0 * clamped);
}
}  // namespace

Image::Image(int width, int height)
    : width_(width), height_(height), pixels_(static_cast<size_t>(width) * height) {}

bool Image::write_png(const std::string& path) const {
    std::vector<std::uint8_t> bytes;
    bytes.reserve(pixels_.size() * 3);
    for (const Color& c : pixels_) {
        bytes.push_back(to_byte(c.x));
        bytes.push_back(to_byte(c.y));
        bytes.push_back(to_byte(c.z));
    }
    return stbi_write_png(path.c_str(), width_, height_, 3, bytes.data(), width_ * 3) != 0;
}

}  // namespace lumen