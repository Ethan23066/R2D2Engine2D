from setuptools import setup, Extension
from Cython.Build import cythonize
import os

print(">>> SETUP GL3 : MODE CORRIGÉ <<<")

# --- FLAGS ---
cpp_flags = ["-O3", "-Wall", "-std=c++2b"]
c_flags   = ["-O3", "-Wall", "-std=c2x"]

# --- INCLUDE DIRS (CORRIGÉ) ---
include_dirs = [
    ".",                                 # racine
    "engine/core/base",
    "engine/core/window",
    "engine/core/inputs",
    "engine/core/renderer",
    "engine/core/backend/gl3",
    "engine/core/backend/gl3/glad",
    "engine/cython/backend/gl3",         # <<< OBLIGATOIRE POUR gl3.pxd
]

# --- EXTENSIONS ---
extensions = [

    # GLAD loader (C)
    Extension(
        "engine.core.backend.gl3.glad_loader",
        sources=["engine/core/backend/gl3/glad/glad.c"],
        include_dirs=include_dirs,
        language="c",
        extra_compile_args=c_flags,
    ),

    # WINDOW
    Extension(
        "engine.cython.window.engine_window",
        sources=[
            "engine/cython/window/engine_window.pyx",
            *[
                os.path.join("engine/core/window", f)
                for f in os.listdir("engine/core/window")
                if f.endswith(".cpp")
            ],
        ],
        include_dirs=include_dirs,
        language="c++",
        extra_compile_args=cpp_flags,
    ),

    # BACKEND GL3
    Extension(
        "engine.cython.backend.gl3.gl3",
        sources=[
            "engine/cython/backend/gl3/gl3.pyx",
            "engine/core/backend/gl3/RendererGL3.cpp",
            "engine/core/backend/gl3/MeshGL3.cpp",
            "engine/core/backend/gl3/ShaderGL3.cpp",
        ],
        include_dirs=include_dirs,
        language="c++",
        extra_compile_args=cpp_flags,
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
