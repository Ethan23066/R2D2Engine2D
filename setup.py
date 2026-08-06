from setuptools import setup, Extension
from Cython.Build import cythonize
from distutils.command.build_ext import build_ext
from distutils.sysconfig import customize_compiler
import os, sys, time, multiprocessing
import re

# ---------------- Progress bar ----------------
def progress_bar(current, total, prefix="Compiling"):
    bar_len = 40
    filled = int(bar_len * current / total) if total else bar_len
    bar = "█" * filled + "░" * (bar_len - filled)
    sys.stdout.write(f"\r{prefix} [{bar}] {current}/{total}")
    sys.stdout.flush()

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

# ---------------- Flags ----------------
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

# ---------------- SCANNER : C++ headers ----------------
def scan_cpp_symbols():
    print("\n>>> Scan des symboles C++ exposés <<<\n")

    hpp_files = []
    for base in CORE_DIRS:
        for root, _, files in os.walk(base):
            for f in files:
                if f.endswith(".hpp"):
                    hpp_files.append(os.path.join(root, f))

    exposed = {}

    sig_re = re.compile(
        r"(?:int|void|bool|float|double)\s+([A-Za-z0-9_:]+)\s*\(([^)]*)\)"
    )

    for hpp in hpp_files:
        with open(hpp, "r", encoding="utf-8") as f:
            content = f.read()

        for match in sig_re.finditer(content):
            func = match.group(1)
            args = match.group(2)
            exposed[func] = {
                "file": hpp,
                "args": args,
                "ref_problem": "&" in args,
            }

    print(f"Trouvé {len(exposed)} fonctions exposées.\n")
    return exposed

# ---------------- SCANNER : .pxd Cython ----------------
def scan_pxd_calls():
    print("\n>>> Scan des appels Cython (.pxd) <<<\n")

    pxd_files = []
    for base in MODULE_DIRS:
        for root, _, files in os.walk(base):
            for f in files:
                if f.endswith(".pxd"):
                    pxd_files.append(os.path.join(root, f))

    calls = {}

    call_re = re.compile(
        r"(?:int|void|bool|float|double)\s+([A-Za-z0-9_]+)\s*\("
    )

    for pxd in pxd_files:
        with open(pxd, "r", encoding="utf-8") as f:
            content = f.read()

        for match in call_re.finditer(content):
            func = match.group(1)
            calls.setdefault(func, []).append(pxd)

    print(f"Trouvé {len(calls)} appels Cython.\n")
    return calls

# ---------------- SCANNER : Analyse du linkage ----------------
def scan_linking(exposed, calls):
    print("\n>>> Analyse du linkage <<<\n")

    for func, info in exposed.items():
        if func not in calls:
            continue

        print(f"- Fonction {func}:")
        print(f"  - Header : {info['file']}")
        print(f"  - Appelée dans : {calls[func]}")

        if info["ref_problem"]:
            print("  ⚠ Signature problématique : référence C++ détectée (const T&)")
            print("    → convertir en pointeur T* ou ajouter un wrapper")

        print()

# ---------------- helpers ----------------
def find_pyx_files(base_dirs):
    out = []
    for base in base_dirs:
        for root, _, files in os.walk(base):
            for f in files:
                if f.endswith(".pyx"):
                    out.append(os.path.join(root, f))
    return out

# ---------------- CORRECTED: collect_core_cpp_for_module ----------------
def collect_core_cpp_for_module(pyx_path):
    parts = pyx_path.replace("\\", "/").split("/")

    # GL3 backend
    if "backend" in parts and "gl3" in parts:
        return [
            "engine/core/backend/gl3/MeshGL3.cpp",
            "engine/core/backend/gl3/RendererGL3.cpp",
            "engine/core/backend/gl3/ShaderGL3.cpp",
            "engine/core/backend/gl3/Texture.cpp",
        ]

    # Window
    if "window" in parts:
        return [
            "engine/core/window/window.cpp",
        ]

    # Base (events, lifecycle, config, log, system, time)
    if "base" in parts:
        return [
            "engine/core/base/config.cpp",
            "engine/core/base/events.cpp",
            "engine/core/base/lifecycle.cpp",
            "engine/core/base/log.cpp",
            "engine/core/base/system.cpp",
            "engine/core/base/time.cpp",
        ]

    # Inputs
    if "input" in parts:
        return [
            "engine/core/inputs/input.cpp",
        ]

    # Renderer (pas de .cpp dans core/renderer)
    if "renderer" in parts:
        return []

    return []

# ---------------- Build class ----------------
class BuildWithGlad(build_ext):
    def build_extensions(self):
        try:
            self.parallel = multiprocessing.cpu_count()
        except Exception:
            self.parallel = 4

        customize_compiler(self.compiler)
        self.mkpath(self.build_temp)

        total = len(self.extensions)
        current = 0

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

        print(f"\n>>> R2D2Engine2D — Compilation (threads = {self.parallel}) <<<\n")

        for ext in self.extensions:
            current += 1
            progress_bar(current, total, prefix=f"Building {ext.name}")

            if getattr(ext, "needs_glad", False) and glad_obj:
                if not getattr(ext, "extra_objects", None):
                    ext.extra_objects = []
                if glad_obj not in ext.extra_objects:
                    ext.extra_objects.append(glad_obj)

            super().build_extension(ext)
            time.sleep(0.02)

        print("\n\n>>> Compilation terminée ! <<<\n")

# ---------------- SCAN avant compilation ----------------
exposed = scan_cpp_symbols()
calls = scan_pxd_calls()
scan_linking(exposed, calls)

# ---------------- Build extensions ----------------
extensions = []

pyx_files = find_pyx_files(MODULE_DIRS)
for pyx in pyx_files:
    module_name = pyx.replace("/", ".").replace(".pyx", "")
    sources = [pyx]

    core_cpp = collect_core_cpp_for_module(pyx)
    sources += core_cpp

    seen = set()
    deduped = []
    for s in sources:
        if s not in seen:
            seen.add(s)
            deduped.append(s)
    sources = deduped

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

    if "backend/gl3" in pyx or "/gl3/" in pyx:
        ext.needs_glad = True
    else:
        ext.needs_glad = False

    extensions.append(ext)

extensions = cythonize(extensions, language_level=3, annotate=False)

setup(
    name="R2D2Engine2D",
    version="1.0",
    ext_modules=extensions,
    cmdclass={"build_ext": BuildWithGlad},
)
