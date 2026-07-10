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
        "engine.cython.window.engine_window",
        sources=[
            "engine/cython/window/engine_window.pyx",
            *collect_cpp("engine/core/window"),
        ],
        include_dirs=["engine/core/window"],
        language="c++",
        libraries=["glfw", "GL"],
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- BASE MODULES ---
    Extension(
        "engine.cython.base.time",
        sources=[
            "engine/cython/base/time.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "engine.cython.base.log",
        sources=[
            "engine/cython/base/log.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "engine.cython.base.system",
        sources=[
            "engine/cython/base/system.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "engine.cython.base.config",
        sources=[
            "engine/cython/base/config.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "engine.cython.base.events",
        sources=[
            "engine/cython/base/events.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    Extension(
        "engine.cython.base.lifecycle",
        sources=[
            "engine/cython/base/lifecycle.pyx",
            *collect_cpp("engine/core/base"),
        ],
        include_dirs=["engine/core/base"],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),

    # --- INPUT ---
    Extension(
        "engine.cython.input.input",
        sources=[
            "engine/cython/input/input.pyx",
            *collect_cpp("engine/core/inputs"),
        ],
        include_dirs=["engine/core/inputs"],
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
        include_dirs=["engine/core/renderer"],
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
