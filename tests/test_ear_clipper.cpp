// tests/test_ear_clipper.cpp — Unit tests for the EarClipper algorithm.

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "ear_clipper.h"
#include "polygon.h"

#define EPSILON 1e-12

static double TotalArea(const std::vector<geo::Triangle> &tris) {
  double sum = 0.0;
  for (const auto &t : tris)
    sum += t.Area();
  return sum;
}

// Triangle test in clockwise order
TEST(EarClipperTest, Triangle_CW_Input) {
  geo::Polygon tri({{0, 0}, {0, 4}, {3, 0}});
  auto tris = geo::EarClipper::Clip(tri);
  EXPECT_EQ(tris.size(), 1u);
  EXPECT_NEAR(TotalArea(tris), 6.0, EPSILON);
}

// Square test
TEST(EarClipperTest, Square) {
  geo::Polygon sq({{0, 0}, {1, 0}, {1, 1}, {0, 1}});
  auto tris = geo::EarClipper::Clip(sq);
  EXPECT_EQ(tris.size(), 2u);
  EXPECT_NEAR(TotalArea(tris), 1.0, EPSILON);
}

// Concave L-shape test
TEST(EarClipperTest, ConcaveLShape) {
  geo::Polygon lshape({{0, 0}, {2, 0}, {2, 1}, {1, 1}, {1, 2}, {0, 2}});
  auto tris = geo::EarClipper::Clip(lshape);
  EXPECT_EQ(tris.size(), 4u);
  EXPECT_NEAR(TotalArea(tris), 3.0, EPSILON);
}

// n-pointed Star: alternating outer (R=1) and inner (r=0.5) radii.
// Area = n*R*r*sin(pi/n).
TEST(EarClipperTest, Star20Vertices) {
  const double PI = std::acos(-1.0);
  const double R = 1.0, r = 0.5;
  const int n = 10;
  std::vector<geo::Vec2> verts;
  verts.reserve(2 * n);
  for (int k = 0; k < 2 * n; ++k) {
    double angle = k * PI / n;
    double radius = (k % 2 == 0) ? R : r;
    verts.push_back({radius * std::cos(angle), radius * std::sin(angle)});
  }
  geo::Polygon star(verts);
  auto tris = geo::EarClipper::Clip(star);
  EXPECT_EQ(tris.size(), 18u);
  EXPECT_NEAR(TotalArea(tris), n * R * r * std::sin(PI / n), EPSILON);
}

// Comb with n teeth: each tooth is tw wide and th tall, sitting on a base of
// height bh. Gaps between teeth are gw wide.
TEST(EarClipperTest, CombShape) {
  const int n = 100;
  const double tw = 1.0, gw = 1.0, th = 20.0, bh = 1.0;
  const double W = n * tw + (n - 1) * gw; // total width

  std::vector<geo::Vec2> verts;
  verts.reserve(4 * n);

  // Bottom edge (left → right)
  verts.push_back({0.0, 0.0});
  verts.push_back({W, 0.0});
  // Right side up; this coincides with the right edge of the last tooth
  verts.push_back({W, bh + th});

  // Zig-zag across the top from right to left
  for (int i = n - 1; i >= 0; --i) {
    const double x_right = i * (tw + gw) + tw;
    const double x_left = i * (tw + gw);

    // Right edge of tooth i — skip for the last tooth (already pushed above)
    if (i < n - 1)
      verts.push_back({x_right, bh + th});

    // Left edge of tooth i (= top-left for i==0, which closes the outline)
    verts.push_back({x_left, bh + th});

    // Drop into the gap on the left, then rise into the next tooth
    if (i > 0) {
      verts.push_back({x_left, bh});                   // gap right wall down
      verts.push_back({(i - 1) * (tw + gw) + tw, bh}); // gap left wall down
    }
  }
  // Left-side descent back to (0,0) is implicit (Polygon closes the outline)

  const double expected_area = W * bh + n * tw * th;

  geo::Polygon comb(verts);
  auto tris = geo::EarClipper::Clip(comb);

  EXPECT_EQ(tris.size(), static_cast<std::size_t>(4 * n - 2));
  EXPECT_NEAR(TotalArea(tris), expected_area, EPSILON);
}

// Very flat rectangle: cross products are proportional to the tiny height,
// so this tests that the convexity check handles near-zero values correctly.
TEST(EarClipperTest, ThinRectangle) {
  geo::Polygon rect({{0, 0}, {100, 0}, {100, 0.00001}, {0, 0.00001}});
  auto tris = geo::EarClipper::Clip(rect);
  EXPECT_EQ(tris.size(), 2u);
  EXPECT_NEAR(TotalArea(tris), 0.001, EPSILON);
}

// A vertex with a 180-degree interior angle (collinear with both neighbors)
// is never an ear. Adjacent ears are clipped first and absorb it.
TEST(EarClipperTest, CollinearMidEdgeVertex) {
  geo::Polygon poly({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {0, 1}});
  auto tris = geo::EarClipper::Clip(poly);
  EXPECT_EQ(tris.size(), 3u);
  EXPECT_NEAR(TotalArea(tris), 2.0, EPSILON);
  for (const auto &t : tris)
    EXPECT_GT(t.Area(), EPSILON);
}

// Two consecutive identical vertices create a zero-length edge.
// The algorithm eventually exhausts all valid ears and throws.
TEST(EarClipperTest, DuplicateConsecutiveVerticesThrows) {
  EXPECT_THROW(
      {
        geo::Polygon poly({{0, 0}, {0, 0}, {1, 0}, {1, 1}, {0, 1}});
        geo::EarClipper::Clip(poly);
      },
      std::runtime_error);
}

// CSV whose last row repeats the first vertex (closed-polygon format).
// The loader must strip the duplicate before triangulation.
TEST(EarClipperTest, ClosedPolygonCSV) {
  const std::string tmp = "/tmp/closed_square_test.csv";
  {
    std::ofstream f(tmp);
    f << "0,0\n1,0\n1,1\n0,1\n0,0\n";
  }
  auto poly = geo::Polygon::LoadFromCSV(tmp);
  EXPECT_EQ(poly.size(), 4u);
  auto tris = geo::EarClipper::Clip(poly);
  EXPECT_NEAR(TotalArea(tris), 1.0, EPSILON);
}
