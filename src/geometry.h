// geometry.h — Basic 2D vector math primitives used throughout the project.
#pragma once

#include <cmath>
#include <ostream>

namespace geo {

// 2D vector struct
struct Vec2 {
  double x = 0.0;
  double y = 0.0;

  friend std::ostream &operator<<(std::ostream &os, const Vec2 &v) {
    return os << "(" << v.x << ", " << v.y << ")";
  }
};

// Signed area of the triangle (a, b, c) * 2.
// Positive  -> counter-clockwise; Negative -> clockwise.
inline double Cross2D(const Vec2 &a, const Vec2 &b, const Vec2 &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Area of triangle (a, b, c).
inline double TriangleArea(const Vec2 &a, const Vec2 &b, const Vec2 &c) {
  return std::abs(Cross2D(a, b, c)) * 0.5;
}

// A triangle is simply three vertices.
struct Triangle {
  Vec2 a, b, c;
  double Area() const { return TriangleArea(a, b, c); }
};

// Returns true if point p is strictly inside triangle t.
// Uses the sign-of-cross-product test
inline bool IsPointInTriangle(const Vec2 &p, const Triangle &t) {
  // A collinear triangle contains no interior points.
  if (std::abs(Cross2D(t.a, t.b, t.c)) < 1e-12)
    return false;

  double d1 = Cross2D(t.a, t.b, p);
  double d2 = Cross2D(t.b, t.c, p);
  double d3 = Cross2D(t.c, t.a, p);

  bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
  bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
  // Point is inside only if all signs agree.
  return !(has_neg && has_pos);
}

} // namespace geo
