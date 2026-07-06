from setuptools import setup, Extension
from Cython.Build import cythonize
import os

# --- FLAGS C++ OPTIMISÉS ---
extra_compile_args = [
    "-O3",
    "-Wall",
    "-std=c++20",
]

extra_link_args = []

# --- FONCTION POUR AUTO-DETECTER LES MODULES ---
def make_ext(module, pyx_path, cpp_path, include_dir, libs=None):
    return Extension(
        module,
        sources=[pyx_path, cpp_path],
        include_dirs=[include_dir],
        language="c++",
        libraries=libs or [],
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )

extensions = [

    # --- WINDOW ---
    make_ext(
        "engine.cython.window.engine_window",
        "engine/cython/window/engine_window.pyx",
        "engine/core/window/engine_window.cpp",
        "engine/core/window",
        libs=["glfw", "GL"]
    ),

    # --- TIME ---
    make_ext(
        "engine.cython.base.time",
        "engine/cython/base/time.pyx",
        "engine/core/base/time.cpp",
        "engine/core/base"
    ),

    # --- LOG ---
    make_ext(
        "engine.cython.base.log",
        "engine/cython/base/log.pyx",
        "engine/core/base/log.cpp",
        "engine/core/base"
    ),

    # --- SYSTEM ---
    make_ext(
        "engine.cython.base.system",
        "engine/cython/base/system.pyx",
        "engine/core/base/system.cpp",
        "engine/core/base"
    ),
]

setup(
    name="r2d2engine2d",
    python_requires="==3.14.*",
    ext_modules=cythonize(
        extensions,
        language_level="3",
        annotate=True,
        force=True,
    ),
)
