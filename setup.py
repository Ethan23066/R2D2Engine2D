from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "engine.cython.window.engine_window",
    sources=[
        "engine/cython/window/engine_window.pyx",
        "engine/core/window/engine_window.cpp",
    ],
    include_dirs=["engine/core/window"],
    language="c++",
    libraries=["glfw", "GL"],
)

setup(
    name="r2d2engine2d",
    ext_modules=cythonize([ext], language_level="3"),
)
