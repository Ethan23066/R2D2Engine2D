from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [

    # --- WINDOW ---
    Extension(
        "engine.cython.window.engine_window",
        sources=[
            "engine/cython/window/engine_window.pyx",
            "engine/core/window/engine_window.cpp",
        ],
        include_dirs=["engine/core/window"],
        language="c++",
        libraries=["glfw", "GL"],
    ),

    # --- TIME ---
    Extension(
        "engine.cython.base.time",
        sources=[
            "engine/cython/base/time.pyx",
            "engine/core/base/time.cpp",
        ],
        include_dirs=["engine/core/base"],
        language="c++",
    ),

    # --- LOG ---
    Extension(
        "engine.cython.base.log",
        sources=[
            "engine/cython/base/log.pyx",
            "engine/core/base/log.cpp",
        ],
        include_dirs=["engine/core/base"],
        language="c++",
    ),

    # --- SYSTEM ---
    Extension(
        "engine.cython.base.system",
        sources=[
            "engine/cython/base/system.pyx",
            "engine/core/base/system.cpp",
        ],
        include_dirs=["engine/core/base"],
        language="c++",
    ),
]

setup(
    name="r2d2engine2d",
    ext_modules=cythonize(extensions, language_level="3"),
)
