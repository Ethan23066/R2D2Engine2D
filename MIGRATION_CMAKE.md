# MIGRATION_CMAKE.md
# Professional build guide for R2D2Engine2D on Linux.
#
# This file documents the official build path for the project as of the
# testing-cmake-migration branch: CMake + nanobind + native GLFW + OpenGL.
#
# The project is designed around native C/C++ and a thin Python layer.
# C/C++ are the hard part, and the build therefore must stay explicit and clear.
# Python is intentionally simple and safe to use.

# -----------------------------------------------------------------------------
# 1) Dependencies
# -----------------------------------------------------------------------------
# On Ubuntu/Debian:
#   sudo apt install build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev python3-dev
#
# On Arch:
#   sudo pacman -S base-devel cmake pkg-config glfw-x11 mesa python
#
# You will also need a Python 3.14 free-threaded build (3.14t) if you want to
# target the no-GIL runtime. The logic is simple: use a CPython build where the
# GIL is not enforced for native threads, and keep access to Python objects
# synchronized explicitly.

# -----------------------------------------------------------------------------
# 2) Configure / build with CMake
# -----------------------------------------------------------------------------
# Linux example:
#   mkdir build
#   cd build
#   cmake -DCMAKE_BUILD_TYPE=Release ..
#   cmake --build . --parallel
#
# The build creates:
#   - a static library: r2d2_core
#   - optional Python extension: _r2d2 (if BUILD_PY_BINDINGS=ON)
#   - optional demo: r2d2_demo (if BUILD_DEMO=ON)

# -----------------------------------------------------------------------------
# 3) Runtime notes
# -----------------------------------------------------------------------------
# 3.1 OpenGL context rules
#   - OpenGL calls must happen on the same thread that owns the current context.
#   - The window wrapper must be created and then make_current() must be called
#     before initializing glad or issuing GL calls.
#
# 3.2 Free-threading / Python 3.14t
#   - The engine is not a pure Python game engine: it is a native engine with a
#     Python layer for access and prototyping.
#   - Do not touch Python objects from native worker threads without your own
#     synchronization. If you need to call Python from workers, push the work to
#     the main thread or guard access with a mutex / queue.
#
# 3.3 GLFW
#   - GLFW is used as a native C API, not through pyglfw.
#   - This is intentional: the engine is mostly native, and the Python layer is
#     only a thin, easy-to-use bridge.

# -----------------------------------------------------------------------------
# 4) Running the demo
# -----------------------------------------------------------------------------
# From build dir:
#   ./r2d2_demo
#
# This opens a small window and clears a color. It is a simple smoke test for
# the engine and demonstrates the intended usage pattern.

# -----------------------------------------------------------------------------
# 5) Python usage
# -----------------------------------------------------------------------------
# Example:
#   import r2d2
#   win = r2d2.Window(800, 600, "R2D2 Demo")
#   win.make_current()
#   renderer = r2d2.RendererGL3()
#   renderer.init(win.native_handle(), 800, 600)
#   while not win.should_close():
#       renderer.begin_frame(0.1, 0.1, 0.2, 1.0)
#       renderer.end_frame()
#       win.swap_buffers()
#       win.poll_events()
#
# For heavy worker threads, do not call back into Python directly; instead,
# queue results back to the render thread or main thread.

# -----------------------------------------------------------------------------
# 6) Build options
# -----------------------------------------------------------------------------
# CMake options:
#   - BUILD_PY_BINDINGS=ON/OFF
#   - BUILD_DEMO=ON/OFF
#
# Example:
#   cmake -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON -DCMAKE_BUILD_TYPE=Debug ..

# -----------------------------------------------------------------------------
# 7) Notes for maintainers
# -----------------------------------------------------------------------------
# - Keep GL calls on the context-owning thread.
# - Keep native and Python ownership clear.
# - Prefer a small, explicit API for binding generation.
# - Document thread and ownership rules in code comments.
