from setuptools import setup, Extension
from Cython.Build import cythonize
import os

# --- COMPILATION FLAGS ---
extra_compile_args = [
    "-O3",
    "-Wall",
    "-std=c++2b",
]

extra_link_args = []

# --- COLLECTEUR DE FICHIERS C++ ---
def collect_cpp(path):
    return [
        os.path.join(path, f)
        for f in os.listdir(path)
        if f.endswith(".cpp")
    ]

# --- INCLUDE DIRS ---
global_include_dirs = [
    ".",  # racine du projet
    "engine/core/base",
    "engine/core/window",
    "engine/core/inputs",
    "engine/core/renderer",
    "engine/core/backend/gl3",
]

# --- EXTENSIONS CYTHON ---
extensions = [

    # --- WINDOW ---
    Extension(
        "engine.cython.window.engine_window",
        sources=[
            "engine/cython/window/engine_window.pyx",
            *collect_cpp("engine/core/window"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- BASE MODULES ---
    *[
        Extension(
            f"engine.cython.base.{name}",
            sources=[
                f"engine/cython/base/{name}.pyx",
                *collect_cpp("engine/core/base"),
            ],
            include_dirs=global_include_dirs,
            language="c++",
            extra_compile_args=extra_compile_args,
            extra_link_args=extra_link_args,
        )
        for name in ["time", "log", "system", "config", "events", "lifecycle"]
    ],

    # --- INPUT ---
    Extension(
        "engine.cython.input.input",
        sources=[
            "engine/cython/input/input.pyx",
            *collect_cpp("engine/core/inputs"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- RENDERER ---
    Extension(
        "engine.cython.renderer.renderer",
        sources=[
            "engine/cython/renderer/renderer.pyx",
            *collect_cpp("engine/core/renderer"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- BACKEND GL3 (VERSION PROPRE) ---
    Extension(
        "engine.cython.backend.gl3.gl3",
        sources=[
            "engine/cython/backend/gl3/gl3.pyx",
            *collect_cpp("engine/core/backend/gl3"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- MANAGER ---
    Extension(
        "engine.cython.backend.manager",
        sources=[
            "engine/cython/backend/manager.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

# --- SETUP ---
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
