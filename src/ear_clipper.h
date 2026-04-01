// ear_clipper.h — Ear-clipping polygon triangulation.
#pragma once

#include <cstddef>
#include <vector>

#include "geometry.h"
#include "polygon.h"

namespace geo {

// Triangulates a simple polygon using the ear-clipping method.
// Time complexity: O(n^2)  |  Space complexity: O(n)
class EarClipper {
public:
  // Returns the (n-2) triangles that partition the polygon.
  // The polygon must be simple (no self-intersections) and counter-clockwise.
  static std::vector<Triangle> Clip(const Polygon &polygon);

private:
  // Returns true when the triangle (prev_idx, curr_idx, next_idx) is an ear:
  //   1. curr is a convex vertex (counter-clockwise cross product > 0).
  //   2. No other remaining vertex lies inside the triangle.
  //
  // next_of defines the circular linked list of the polygon's current
  // (remaining) vertex set, so only live vertices are tested.
  //
  // Time complexity: O(n)  |  Space complexity: O(1)
  static bool IsEar(std::size_t prev_idx, std::size_t curr_idx,
                    std::size_t next_idx, const std::vector<Vec2> &verts,
                    const std::vector<std::size_t> &next_of);
};

} // namespace geo
