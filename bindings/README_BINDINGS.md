# Build metadata for Python bindings (enable nanobind)
# This file is optional and documents how to build the Python extension.
# Use the project's CMakeLists.txt (testing-cmake-migration branch) which already
# fetches nanobind and creates the _r2d2 target.

# Example (from repo root):
# mkdir -p build && cd build
# cmake -DBUILD_PY_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release ..
# cmake --build . --parallel
