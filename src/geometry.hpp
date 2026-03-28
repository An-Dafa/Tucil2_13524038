#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cmath>
#include <algorithm>

struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Operasi vektor dasar
    Vector3 operator+(const Vector3& v) const { 
        return Vector3(x + v.x, y + v.y, z + v.z); 
    }

    Vector3 operator-(const Vector3& v) const { 
        return Vector3(x - v.x, y - v.y, z - v.z); 
    }

    Vector3 operator*(float s) const { 
        return Vector3(x * s, y * s, z * s); 
    }

    Vector3 operator/(float s) const { 
        return Vector3(x / s, y / s, z / s); 
    }

    // Perkalian titik dan silang
    float dot(const Vector3& v) const { 
        return x * v.x + y * v.y + z * v.z; 
    }

    Vector3 cross(const Vector3& v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    // Panjang vektor dan normalisasi
    float length() const { 
        return std::sqrt(x*x + y*y + z*z); 
    }

    Vector3 normalize() const {
        float len = length();
        if (len > 0) return *this / len;
        return *this;
    }
};

struct Triangle {
    Vector3 v0, v1, v2;
};

struct BoundingBox {
    Vector3 min;
    Vector3 max;

    BoundingBox() {
        min = Vector3(1e9, 1e9, 1e9);
        max = Vector3(-1e9, -1e9, -1e9);
    }

    Vector3 getCenter() const {
        return Vector3((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, (min.z + max.z) / 2.0f);
    }

    Vector3 getSize() const {
        return max - min;
    }
};

#endif