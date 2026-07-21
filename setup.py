from setuptools import setup, Extension
from Cython.Build import cythonize
import os

# --- BARRE DE PROGRESSION ---
def progress_bar(step, total, label=""):
    width = 24
    filled = int(width * (step / total))
    bar = "█" * filled + "░" * (width - filled)
    print(f"[SETUP] Progress: |{bar}| {int(step/total*100)}% - {label}")

TOTAL_STEPS = 10
step = 0

# --- FLAGS ---
step += 1
progress_bar(step, TOTAL_STEPS, "Chargement des flags")

cpp_flags = ["-O3", "-Wall", "-std=c++2b"]   # C++26
c_flags   = ["-O3", "-Wall", "-std=c2x"]     # C23
extra_link_args = []

# --- COLLECTEUR ---
step += 1
progress_bar(step, TOTAL_STEPS, "Initialisation collecteur .cpp")

def collect_cpp(path):
    print(f"[SETUP] Collecte des .cpp dans {path}...")
    files = [
        os.path.join(path, f)
        for f in os.listdir(path)
        if f.endswith(".cpp")
    ]
    print(f"[SETUP]   -> {len(files)} fichiers trouvés... OK")
    return files

# --- INCLUDE DIRS ---
step += 1
progress_bar(step, TOTAL_STEPS, "Chargement include dirs")

global_include_dirs = [
    ".",
    "engine/core/base",
    "engine/core/window",
    "engine/core/inputs",
    "engine/core/renderer",
    "engine/core/backend/gl3",
    "engine/core/backend/gl3/glad",
]

# --- EXTENSIONS ---
step += 1
progress_bar(step, TOTAL_STEPS, "Initialisation extensions")

extensions = []

# --- GLAD (C23) ---
step += 1
progress_bar(step, TOTAL_STEPS, "Module GLAD (C23)")

extensions.append(
    Extension(
        "engine.core.backend.gl3.glad_loader",
        sources=["engine/core/backend/gl3/glad/glad.c"],
        include_dirs=global_include_dirs,
        language="c",
        extra_compile_args=c_flags,
    )
)

# --- WINDOW ---
step += 1
progress_bar(step, TOTAL_STEPS, "Module WINDOW")

extensions.append(
    Extension(
        "engine.cython.window.engine_window",
        sources=[
            "engine/cython/window/engine_window.pyx",
            *collect_cpp("engine/core/window"),
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=cpp_flags,
        extra_link_args=extra_link_args,
    )
)

# --- BASE MODULES ---
step += 1
progress_bar(step, TOTAL_STEPS, "Modules BASE")

for name in ["time", "log", "system", "config", "events", "lifecycle"]:
    extensions.append(
        Extension(
            f"engine.cython.base.{name}",
            sources=[
                f"engine/cython/base/{name}.pyx",
                *collect_cpp("engine/core/base"),
            ],
            include_dirs=global_include_dirs,
            language="c++",
            extra_compile_args=cpp_flags,
            extra_link_args=extra_link_args,
        )
    )

# --- BACKEND GL3 ---
step += 1
progress_bar(step, TOTAL_STEPS, "Backend GL3 (C++26)")

extensions.append(
    Extension(
        "engine.cython.backend.gl3.gl3",
        sources=[
            "engine/cython/backend/gl3/gl3.pyx",
            "engine/core/backend/gl3/RendererGL3.cpp",
            "engine/core/backend/gl3/MeshGL3.cpp",
            "engine/core/backend/gl3/ShaderGL3.cpp",
        ],
        include_dirs=global_include_dirs,
        language="c++",
        extra_compile_args=cpp_flags,
        extra_link_args=extra_link_args,
    )
)

# --- SETUP ---
step += 1
progress_bar(step, TOTAL_STEPS, "Compilation CYTHON")

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

progress_bar(TOTAL_STEPS, TOTAL_STEPS, "Build terminé")
