#pragma once

#include <cmath>
#include <ostream>

namespace lumen {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    constexpr Vec3() = default;
    constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    constexpr Vec3 operator-() const { return {-x, -y, -z}; }

    constexpr double operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    constexpr double& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }

    constexpr Vec3& operator+=(const Vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr Vec3& operator-=(const Vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    constexpr Vec3& operator*=(double t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    constexpr Vec3& operator/=(double t) { return *this *= 1.0 / t; }

    constexpr double length_squared() const { return x * x + y * y + z * z; }
    double length() const { return std::sqrt(length_squared()); }
};

// Same type, different meaning. Makes signatures self-documenting.
using Point3 = Vec3;
using Color = Vec3;

// ---------- Arithmetic ----------

constexpr Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
constexpr Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }

// Component-wise multiply (used for tinting colours)
constexpr Vec3 operator*(const Vec3& a, const Vec3& b) { return {a.x * b.x, a.y * b.y, a.z * b.z}; }

constexpr Vec3 operator*(double t, const Vec3& v) { return {t * v.x, t * v.y, t * v.z}; }
constexpr Vec3 operator*(const Vec3& v, double t) { return t * v; }
constexpr Vec3 operator/(const Vec3& v, double t) { return (1.0 / t) * v; }

// ---------- Vector maths ----------

constexpr double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

constexpr Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

inline Vec3 unit_vector(const Vec3& v) { return v / v.length(); }

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

}  // namespace lumen