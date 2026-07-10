from setuptools import setup, Extension
from Cython.Build import cythonize
import os

extra_compile_args = [
    "-O3",
    "-Wall",
    "-std=c++2b",
]

extra_link_args = []

def collect_cpp(path):
    """Collect all .cpp files in a directory."""
    return [
        os.path.join(path, f)
        for f in os.listdir(path)
        if f.endswith(".cpp")
    ]

extensions = [

    # --- WINDOW ---
    Extension(
        "base.cython.window.engine_window",
        sources=[
            "base/cython/window/engine_window.pyx",
            *collect_cpp("engine/core/window"),
        ],
        include_dirs=["base/core/window"],
        language="c++",
        libraries=["glfw", "GL"],
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- BASE MODULES (time, log, system, config, base, events, lifecycle) ---
    Extension(
        "base.cython.base.time",
        sources=[
            "base/cython/base/time.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.log",
        sources=[
            "base/cython/base/log.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.system",
        sources=[
            "base/cython/base/system.pyx",
            *collect_cpp("engine/core/base"),  # ← LOG.CPP INCLUS AUTOMATIQUEMENT
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.config",
        sources=[
            "base/cython/base/config.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.base",
        sources=[
            "base/cython/base/base.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.events",
        sources=[
            "base/cython/base/events.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "base.cython.base.lifecycle",
        sources=[
            "base/cython/base/lifecycle.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["base/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- INPUT ---
    Extension(
        "base.cython.input.input",
        sources=[
            "base/cython/input/input.pyx",
            *collect_cpp("engine/core/inputs"),
        ],
        include_dirs=["base/core/inputs"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- RENDERER ---
    Extension(
        "base.cython.renderer.renderer",
        sources=[
            "base/cython/renderer/renderer.pyx",
            *collect_cpp("engine/core/renderer"),
        ],
        include_dirs=["base/core/renderer"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
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
