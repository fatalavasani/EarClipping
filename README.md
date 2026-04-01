# Polygon Triangulator — C++ Ear Clipping

A C++17 command-line tool that triangulates a simple concave polygon from a CSV
file using the **ear-clipping method**, then prints the triangles and their summed area.

---

## Build

Requires a C++17-capable compiler (GCC or Clang with `-std=c++17`) and Google Test. 

```bash
make        # compiles polygon_triangulator into build/
```

---

## Run

```bash
./build/polygon_triangulator data/simple_concave_poly.csv
./build/polygon_triangulator data/concave_poly.csv
```

**Expected output:**

| File                      | Expected Total Area          |
|---------------------------|------------------------------|
| `simple_concave_poly.csv` | 0.65                         |
| `concave_poly.csv`        | 1269.1874283009124           |

### CSV format

One vertex per row, optional header, two comma-separated `x,y` values.
The last row may repeat the first vertex, the loader strips the duplicate automatically.

---

## Tests

**Prerequisite:** [Google Test](https://github.com/google/googletest) must be installed on your system.

```bash
# macOS
brew install googletest

# Ubuntu / Debian
sudo apt install libgtest-dev
```

```bash
make test   # builds and runs both test suites
```

---

## Algorithm — Ear Clipping

Reference: [Wikipedia — Polygon triangulation](https://en.wikipedia.org/wiki/Polygon_triangulation)

1. **Load** the polygon from CSV; enforce counter-clockwise orientation (shoelace sign).
2. **Pre-compute** `is_ear[]` for every vertex — O(n^2) total.
3. **While** vertices > 3:
   - Advance a pointer to the next vertex marked as an ear.
   - **Clip** it: record the triangle, remove it from the linked list in O(1).
   - **Re-evaluate only the two neighbors** — the only vertices whose ear
     status can change after a clip (simple polygon property O(n)).
4. The last 3 vertices form the final triangle.


### Complexity

| | |
|-|-|
| **Time** | O(n^2) — n clips * O(n) is-ear re-evaluation per clip |
| **Space** | O(n) — linked-list arrays + triangle list |

---

## Project Structure

```
.
├── Makefile
├── README.md
├── data/
│   ├── simple_concave_poly.csv   # area ≈ 0.65
│   └── concave_poly.csv          # area ≈ 1269.19
├── src/
│   ├── geometry.h        # Vec2, Cross2D, Triangle, IsPointInTriangle
│   ├── polygon.h/.cpp    # Polygon type + CSV loader
│   ├── ear_clipper.h/.cpp# EarClipper algorithm
│   └── main.cpp          # CLI entry point
└── tests/
    ├── test_geometry.cpp     # Geometry primitive tests
    └── test_ear_clipper.cpp  # Algorithm tests
```
