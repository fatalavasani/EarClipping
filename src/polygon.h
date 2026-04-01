// polygon.h — Represents a simple polygon and handles CSV loading.
#pragma once

#include <string>
#include <vector>

#include "geometry.h"

namespace geo {

// A simple polygon stored as an ordered list of vertices.
// The constructor ensures counter-clockwise orientation
class Polygon {
public:
  // Constructs a polygon from a list of vertices.
  // Will reorder to counter-clockwise if the area is negative.
  explicit Polygon(std::vector<Vec2> vertices);

  // Loads a polygon from a CSV file with an optional header row.
  static Polygon LoadFromCSV(const std::string &path);
  // Returns the vertices of the polygon.
  const std::vector<Vec2> &vertices() const { return vertices_; }
  // Returns the number of vertices of the polygon.
  std::size_t size() const { return vertices_.size(); }

private:
  // The vertices of the polygon in counter-clockwise order.
  std::vector<Vec2> vertices_;

  // Returns the signed area; negative means clockwise orientation.
  static double SignedArea(const std::vector<Vec2> &verts);
};

} // namespace geo
