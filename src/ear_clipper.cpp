// ear_clipper.cpp — O(n^2) ear-clipping triangulation.
//
// The polygon is kept in a circular doubly-linked list so clipping a vertex
// is O(1). is_ear[] is pre-computed for all n vertices in O(n^2) by scanning
// remaining vertices for each candidate. After each clip, only the two
// neighbors can change ear status, so only they are re-checked in O(n).
// Repeating for n-2 ears gives O(n^2) overall.

#include "ear_clipper.h"

#include <stdexcept>

namespace geo {

bool EarClipper::IsEar(std::size_t prev_idx, std::size_t curr_idx,
                       std::size_t next_idx, const std::vector<Vec2> &verts,
                       const std::vector<std::size_t> &next_of) {
  const Vec2 &prev = verts[prev_idx];
  const Vec2 &curr = verts[curr_idx];
  const Vec2 &next = verts[next_idx];

  // Condition 1: curr must be a convex vertex (positive cross product)
  if (Cross2D(prev, curr, next) <= 0.0)
    return false;

  // Condition 2: no other remaining vertex may lie inside the ear triangle.
  // Start the scan just after `next` and stop just before `prev`.
  std::size_t i = next_of[next_idx];
  while (i != prev_idx) {
    if (IsPointInTriangle(verts[i], {prev, curr, next}))
      return false;
    i = next_of[i];
  }
  return true;
}

std::vector<Triangle> EarClipper::Clip(const Polygon &polygon) {
  if (polygon.size() < 3)
    throw std::runtime_error("Polygon must have at least 3 vertices.");

  const std::vector<Vec2> &verts = polygon.vertices();
  const std::size_t n = verts.size();

  // Build circular doubly-linked list
  // prev_of[i] / next_of[i] are the predecessor/successor indices in the
  // current shrinking polygon.
  std::vector<std::size_t> prev_of(n), next_of(n);
  for (std::size_t i = 0; i < n; ++i) {
    prev_of[i] = (i + n - 1) % n;
    next_of[i] = (i + 1) % n;
  }

  // Pre-compute ear status for every vertex
  std::vector<bool> is_ear(n);
  for (std::size_t i = 0; i < n; ++i)
    is_ear[i] = IsEar(prev_of[i], i, next_of[i], verts, next_of);

  // Main clipping loop
  std::vector<Triangle> triangles;
  triangles.reserve(n - 2);

  std::size_t remaining = n;
  std::size_t curr = 0; // Advances through the linked list.

  while (remaining > 3) {
    // Find the next ear starting from curr
    std::size_t steps = 0;
    while (!is_ear[curr]) {
      curr = next_of[curr];
      if (++steps > remaining) {
        // Should never happen for a valid simple polygon.
        throw std::runtime_error(
            "Ear-clipping failed: no ear found. "
            "The polygon may be self-intersecting or degenerate.");
      }
    }

    const std::size_t prev = prev_of[curr];
    const std::size_t next = next_of[curr];

    // Record the ear triangle.
    triangles.push_back({verts[prev], verts[curr], verts[next]});

    // Clip: remove `curr` from the linked list
    next_of[prev] = next;
    prev_of[next] = prev;
    --remaining;

    // Re-evaluate only the two neighbors
    // All other vertices' ear status is unchanged (simple polygon property).
    is_ear[prev] = IsEar(prev_of[prev], prev, next_of[prev], verts, next_of);
    is_ear[next] = IsEar(prev_of[next], next, next_of[next], verts, next_of);

    curr = next; // Continue forward through the list.
  }

  // Final triangle: 3 vertices remain, emit directly.
  const std::size_t prev = next_of[curr];
  const std::size_t next = next_of[prev];
  triangles.push_back({verts[prev], verts[curr], verts[next]});

  return triangles;
}

} // namespace geo
