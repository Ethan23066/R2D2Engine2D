from setuptools import setup, Extension
from Cython.Build import cythonize
from distutils.command.build_ext import build_ext
import os
import sys
import time

# ---------------- Barre de progression ----------------

def progress_bar(current, total, prefix="Compiling"):
    bar_len = 40
    filled = int(bar_len * current / total)
    bar = "█" * filled + "░" * (bar_len - filled)
    sys.stdout.write(f"\r{prefix} [{bar}] {current}/{total}")
    sys.stdout.flush()


class BuildWithProgress(build_ext):
    def build_extensions(self):
        total = len(self.extensions)
        current = 0

        print("\n>>> R2D2Engine2D — Compilation complète <<<\n")

        for ext in self.extensions:
            current += 1
            progress_bar(current, total, prefix=f"Building {ext.name}")
            super().build_extension(ext)
            time.sleep(0.05)

        print("\n\n>>> Compilation terminée ! <<<\n")


# ---------------- Scan utilitaire ----------------

def scan(folder, ext):
    out = []
    for root, dirs, files in os.walk(folder):
        for f in files:
            if f.endswith(ext):
                out.append(os.path.join(root, f))
    return out


# ---------------- Dossiers EXACTEMENT comme ton tree ----------------

MODULE_DIRS = [
    "engine/cython/backend/gl3",
    "engine/cython/backend",
    "engine/cython/window",
    "engine/cython/base",
    "engine/cython/input",
    "engine/cython/renderer",
]

CORE_DIRS = [
    "engine/core/backend/gl3",
    "engine/core/backend/gl3/glad",
    "engine/core/window",
    "engine/core/base",
    "engine/core/inputs",
    "engine/core/renderer",
]

INCLUDE_DIRS = ["." ] + MODULE_DIRS + CORE_DIRS

CPP_FLAGS = [
    "-std=c++2b",
    "-O3",
    "-Wall",
    "-march=ivybridge",
    "-mtune=ivybridge",
    "-fPIC",
]

extensions = []

# ---------------- GLAD (C pur) ----------------

extensions.append(
    Extension(
        "engine.core.backend.gl3.glad_loader",
        sources=["engine/core/backend/gl3/glad/glad.c"],
        include_dirs=INCLUDE_DIRS,
        extra_compile_args=["-O3", "-Wall"],
        language="c",
    )
)

# ---------------- Compilation totale : chaque .pyx devient une extension ----------------

for module_dir in MODULE_DIRS:

    pyx_files = scan(module_dir, ".pyx")
    cpp_files = scan(module_dir, ".cpp")

    # ⚠️ Ne jamais compiler les .cpp générés par Cython
    cpp_files = [
        c for c in cpp_files
        if "engine/cython" not in c
    ]

    # Ajoute aussi les .cpp du core correspondant
    for core_dir in CORE_DIRS:
        if module_dir.split("/")[-1] in core_dir:
            cpp_files += scan(core_dir, ".cpp")

    for pyx in pyx_files:
        module_name = pyx.replace("/", ".").replace(".pyx", "")

        extensions.append(
            Extension(
                module_name,
                sources=[pyx] + cpp_files,
                include_dirs=INCLUDE_DIRS,
                extra_compile_args=CPP_FLAGS,
                language="c++",
            )
        )


# ---------------- Compilation Cython ----------------

extensions = cythonize(
    extensions,
    language_level=3,
    annotate=True,
)

# ---------------- Setup ----------------

setup(
    name="R2D2Engine2D",
    version="1.0",
    ext_modules=extensions,
    cmdclass={"build_ext": BuildWithProgress},
)
