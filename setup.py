from setuptools import setup, Extension
from Cython.Build import cythonize
from distutils.command.build_ext import build_ext
from distutils.sysconfig import customize_compiler
import os, sys, time

# ---------------- Progress bar ----------------

def progress_bar(current, total, prefix="Compiling"):
    bar_len = 40
    filled = int(bar_len * current / total) if total else bar_len
    bar = "█" * filled + "░" * (bar_len - filled)
    sys.stdout.write(f"\r{prefix} [{bar}] {current}/{total}")
    sys.stdout.flush()


class BuildWithGlad(build_ext):
    def build_extensions(self):
        # préparer le compilateur
        customize_compiler(self.compiler)
        self.mkpath(self.build_temp)

        total = len(self.extensions)
        current = 0

        # Précompile glad.c (une seule fois) en objet C
        GLAD_C = "engine/core/backend/gl3/glad/glad.c"
        glad_obj = None
        if os.path.exists(GLAD_C):
            try:
                objs = self.compiler.compile(
                    [GLAD_C],
                    output_dir=self.build_temp,
                    include_dirs=INCLUDE_DIRS,
                    extra_postargs=C_FLAGS,
                )
                if objs:
                    glad_obj = objs[0]
                    print(f"\nPrecompiled glad: {glad_obj}")
            except Exception as e:
                print("\nWarning: failed to precompile glad.c:", e)

        print("\n>>> R2D2Engine2D — Compilation <<<\n")
        for ext in self.extensions:
            current += 1
            progress_bar(current, total, prefix=f"Building {ext.name}")

            # si l'extension a besoin de glad, lui attacher l'objet compilé
            if getattr(ext, "needs_glad", False) and glad_obj:
                if not getattr(ext, "extra_objects", None):
                    ext.extra_objects = []
                if glad_obj not in ext.extra_objects:
                    ext.extra_objects.append(glad_obj)

            # déléguer la compilation/link standard
            super().build_extension(ext)
            time.sleep(0.02)

        print("\n\n>>> Compilation terminée ! <<<\n")


# ---------------- helpers ----------------

def find_pyx_files(base_dirs):
    out = []
    for base in base_dirs:
        for root, _, files in os.walk(base):
            for f in files:
                if f.endswith(".pyx"):
                    out.append(os.path.join(root, f))
    return out


def collect_core_cpp_for_module(pyx_path):
    # Retourne une liste de .cpp core à ajouter pour ce module
    # (ajuste ici si tu veux plus finement inclure des fichiers)
    parts = pyx_path.replace("\\", "/").split("/")
    # si module dans backend/gl3, on ajoute les core gl3 .cpp
    if "backend" in parts and "gl3" in parts:
        return [
            "engine/core/backend/gl3/MeshGL3.cpp",
            "engine/core/backend/gl3/RendererGL3.cpp",
            "engine/core/backend/gl3/ShaderGL3.cpp",
            "engine/core/backend/gl3/Texture.cpp",
        ]
    # si module est window binding, ajouter core/window
    if "window" in parts:
        return ["engine/core/window/window.cpp"]
    # par défaut aucun core cpp ajouté
    return []


# ---------------- Directories ----------------
MODULE_DIRS = [
    "engine/cython/backend",
    "engine/cython/backend/gl3",
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

INCLUDE_DIRS = ["."] + MODULE_DIRS + CORE_DIRS

# ---------------- Flags (utilise les tiens) ----------------
CPP_FLAGS = [
    "-std=c++2b",
    "-O3",
    "-Wall",
    "-march=ivybridge",
    "-mtune=ivybridge",
    "-fPIC",
]

C_FLAGS = [
    "-std=c23",
    "-O3",
    "-Wall",
    "-fPIC",
]

# ---------------- Build extensions ----------------
extensions = []

pyx_files = find_pyx_files(MODULE_DIRS)
for pyx in pyx_files:
    # module name derived from path, ex: engine.cython.backend.gl3.gl3
    module_name = pyx.replace("/", ".").replace(".pyx", "")
    # sources start with the pyx itself (cythonize will generate .cpp)
    sources = [pyx]

    # add related core cpp sources when appropriate
    core_cpp = collect_core_cpp_for_module(pyx)
    sources += core_cpp

    # dedupe sources (preserve order)
    seen = set()
    deduped = []
    for s in sources:
        if s not in seen:
            seen.add(s)
            deduped.append(s)
    sources = deduped

    # libraries: window modules need glfw
    libs = ["GL"]
    if "window" in pyx:
        libs.append("glfw")

    ext = Extension(
        module_name,
        sources=sources,
        include_dirs=INCLUDE_DIRS,
        extra_compile_args=CPP_FLAGS,
        language="c++",
        libraries=libs,
    )

    # mark whether this extension needs glad.o linked
    if "backend/gl3" in pyx or "/gl3/" in pyx:
        ext.needs_glad = True
    else:
        ext.needs_glad = False

    extensions.append(ext)

# Cythonize (génère .cpp pour chaque .pyx)
extensions = cythonize(extensions, language_level=3, annotate=False)

setup(
    name="R2D2Engine2D",
    version="1.0",
    ext_modules=extensions,
    cmdclass={"build_ext": BuildWithGlad},
)
