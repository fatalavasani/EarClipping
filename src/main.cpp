// main.cpp — Entry point: load polygon from CSV, run ear clipping, print results.
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "ear_clipper.h"
#include "polygon.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: polygon_triangulator <path/to/polygon.csv>\n";
    return EXIT_FAILURE;
  }

  const std::string path = argv[1];

  try {
    const geo::Polygon polygon = geo::Polygon::LoadFromCSV(path);
    const std::vector<geo::Triangle> triangles = geo::EarClipper::Clip(polygon);

    std::cout << std::fixed << std::setprecision(13);

    double total_area = 0.0;
    for (std::size_t i = 0; i < triangles.size(); ++i) {
      const auto& t = triangles[i];
      const double area = t.Area();
      total_area += area;
      std::cout << "Triangle " << (i + 1) << ": "
                << t.a << ", " << t.b << ", " << t.c
                << "  area=" << area << "\n";
    }

    std::cout << "\nTotal area: " << total_area << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
