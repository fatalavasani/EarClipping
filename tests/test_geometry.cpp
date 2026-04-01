// tests/test_geometry.cpp — Unit tests for geometry primitives.

#include <cmath>

#include <gtest/gtest.h>

#include "geometry.h"

#define EPSILON 1e-12

// ---- Cross2D ---------------------------------------------------------------

TEST(Cross2DTest, CCWTrianglePositive) {
  EXPECT_GT(geo::Cross2D({0, 0}, {1, 0}, {0, 1}), 0.0);
}

TEST(Cross2DTest, CWTriangleNegative) {
  EXPECT_LT(geo::Cross2D({0, 0}, {0, 1}, {1, 0}), 0.0);
}

TEST(Cross2DTest, CollinearZero) {
  EXPECT_EQ(geo::Cross2D({0, 0}, {1, 1}, {2, 2}), 0.0);
}

// ---- TriangleArea ----------------------------------------------------------

TEST(TriangleAreaTest, UnitRightTriangle) {
  EXPECT_NEAR(geo::TriangleArea({0, 0}, {1, 0}, {0, 1}), 0.5, EPSILON);
}

TEST(TriangleAreaTest, ThreeFourFiveTriangle) {
  EXPECT_NEAR(geo::TriangleArea({0, 0}, {3, 0}, {0, 4}), 6.0, EPSILON);
}

TEST(TriangleAreaTest, CollinearIsZero) {
  EXPECT_NEAR(geo::TriangleArea({0, 0}, {1, 1}, {2, 2}), 0.0, EPSILON);
}

// ---- IsPointInTriangle -----------------------------------------------------

TEST(IsPointInTriangleTest, CentroidIsInside) {
  geo::Triangle t{{0, 0}, {2, 0}, {1, 2}};
  EXPECT_TRUE(geo::IsPointInTriangle({1.0, 2.0 / 3.0}, t));
}

TEST(IsPointInTriangleTest, FarPointIsOutside) {
  geo::Triangle t{{0, 0}, {2, 0}, {1, 2}};
  EXPECT_FALSE(geo::IsPointInTriangle({5, 5}, t));
}

TEST(IsPointInTriangleTest, LeftOfEdgeIsOutside) {
  geo::Triangle t{{0, 0}, {2, 0}, {1, 2}};
  EXPECT_FALSE(geo::IsPointInTriangle({-1, 0}, t));
}

TEST(IsPointInTriangleTest, VertexIsNotOutside) {
  geo::Vec2 a{0, 0};
  geo::Triangle t{a, {2, 0}, {1, 2}};
  EXPECT_TRUE(geo::IsPointInTriangle(a, t));
}
