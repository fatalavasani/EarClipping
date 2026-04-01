// polygon.cpp — Polygon implementation: construction, orientation, CSV parsing.
#include "polygon.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace geo {

// Internal helpers - Using shoelace method to find the signed area

double Polygon::SignedArea(const std::vector<Vec2> &verts) {
  double area = 0.0;
  const std::size_t n = verts.size();
  for (std::size_t i = 0; i < n; ++i) {
    const Vec2 &cur = verts[i];
    const Vec2 &nxt = verts[(i + 1) % n];
    area += (cur.x * nxt.y) - (nxt.x * cur.y);
  }
  return area * 0.5; // Positive -> Counter-Clockwise, Negative -> Clockwise
}

// Polygon - Stores vertices in counter-clockwise order with no duplicated
// closing points
Polygon::Polygon(std::vector<Vec2> vertices) : vertices_(std::move(vertices)) {
  if (vertices_.size() < 3) {
    throw std::runtime_error("A polygon must have at least 3 vertices.");
  }
  // Ensure counter-clockwise orientation: reverse if clockwise.
  if (SignedArea(vertices_) < 0.0) {
    std::reverse(vertices_.begin(), vertices_.end());
  }
}

Polygon Polygon::LoadFromCSV(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + path);
  }

  std::vector<Vec2> verts;
  std::string line;
  bool first_line = true;

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    // Skip header row if it starts with a non-numeric character.
    if (first_line) {
      first_line = false;
      char c = line.front();
      if (!std::isdigit(c) && c != '-' && c != '+' && c != '.')
        continue;
    }

    std::istringstream ss(line);
    std::string tok_x, tok_y;
    if (!std::getline(ss, tok_x, ',') || !std::getline(ss, tok_y, ',')) {
      throw std::runtime_error("Malformed CSV row: " + line);
    }

    try {
      verts.push_back({std::stod(tok_x), std::stod(tok_y)});
    } catch (const std::exception &) {
      throw std::runtime_error("Non-numeric value in CSV row: " + line);
    }
  }

  // Drop the closing vertex if it duplicates the first.
  if (verts.size() >= 2) {
    const Vec2 &first = verts.front();
    const Vec2 &last = verts.back();
    if (first.x == last.x && first.y == last.y)
      verts.pop_back();
  }

  return Polygon(std::move(verts));
}

} // namespace geo
