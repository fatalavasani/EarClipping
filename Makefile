CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEPFLAGS := -MMD -MP

SRC  := src
TEST := tests
BLD  := build

# Google Test — must be installed system-wide before running tests.
# macOS:  brew install googletest
# Linux:  sudo apt install libgtest-dev
GTEST_LIBS := -lgtest_main -lgtest

LIB_SRCS := $(SRC)/polygon.cpp $(SRC)/ear_clipper.cpp
LIB_OBJS := $(LIB_SRCS:$(SRC)/%.cpp=$(BLD)/%.o)

BIN      := $(BLD)/polygon_triangulator
TEST_GEO := $(BLD)/test_geometry
TEST_EC  := $(BLD)/test_ear_clipper

.PHONY: all test clean

all: $(BIN)

$(BLD):
	mkdir -p $(BLD)

# ── Library objects ──────────────────────────────────────────────────────────

$(BLD)/%.o: $(SRC)/%.cpp | $(BLD)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(SRC) -c $< -o $@

# ── Main executable ──────────────────────────────────────────────────────────

$(BLD)/main.o: $(SRC)/main.cpp | $(BLD)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(SRC) -c $< -o $@

$(BIN): $(BLD)/main.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ── Test executables ─────────────────────────────────────────────────────────

$(BLD)/test_geometry.o: $(TEST)/test_geometry.cpp | $(BLD)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(SRC) -c $< -o $@

$(TEST_GEO): $(BLD)/test_geometry.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(GTEST_LIBS) -o $@

$(BLD)/test_ear_clipper.o: $(TEST)/test_ear_clipper.cpp | $(BLD)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(SRC) -c $< -o $@

$(TEST_EC): $(BLD)/test_ear_clipper.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(GTEST_LIBS) -o $@

# ── Run all tests ─────────────────────────────────────────────────────────────

test: $(TEST_GEO) $(TEST_EC)
	@$(TEST_GEO) --gtest_color=yes
	@$(TEST_EC)  --gtest_color=yes

# ── Cleanup ───────────────────────────────────────────────────────────────────

clean:
	rm -rf $(BLD)

# Auto-generated header dependencies
-include $(wildcard $(BLD)/*.d)
