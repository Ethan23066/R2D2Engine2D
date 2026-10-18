// from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from pathlib import Path
import sys
import sysconfig
import os

ROOT = Path(__file__).parent

# Where we look for nanobind sources
NANOBIND_SRC_DIR = ROOT / "engine" / "nanobind"

# Fallback include if nanobind not installed (vendor/)
def get_nanobind_include():
    try:
        import nanobind
        return nanobind.get_include()
    except Exception:
        fallback = ROOT / "vendor" / "nanobind" / "include"
        return str(fallback)

NB_INCLUDE = get_nanobind_include()

# Engine include dirs (adjust if your headers live elsewhere)
INCLUDE_DIRS = [
    NB_INCLUDE,
    str(ROOT),
    str(ROOT / "engine" / "core"),
]

# Compiler flags (platform-aware)
if sys.platform == "win32":
    CPP_FLAGS = ["/std:c++20", "/O2"]
    LINK_ARGS = []
else:
    CPP_FLAGS = ["-std=c++20", "-O3", "-Wall", "-fPIC", "-march=native"]
    LINK_ARGS = []

# Helper: discover .cpp sources under engine/nanobind and map to module names
def discover_nanobind_extensions(src_root: Path):
    modules = {}
    if not src_root.exists():
        return modules

    for p in sorted(src_root.rglob("*.cpp")):
        rel = p.relative_to(ROOT)
        # build a Python module name from path: engine/nanobind/foo/bar.cpp -> engine.nanobind.foo.bar
        mod_name = ".".join(rel.with_suffix("").parts)
        modules.setdefault(mod_name, []).append(str(p))
    return modules

# Build class: single-threaded, optional precompile of glad.c
class BuildNoParallel(build_ext):
    def build_extensions(self):
        # force single-threaded builds (no multiprocessing)
        self.parallel = 1

        # try to precompile glad.c if present (keeps legacy optimisation)
        glad_c = ROOT / "engine" / "core" / "backend" / "gl3" / "glad" / "glad.c"
        glad_obj = None
        try:
            if glad_c.exists():
                objs = self.compiler.compile(
                    [str(glad_c)],
                    output_dir=self.build_temp,
                    include_dirs=INCLUDE_DIRS,
                    extra_postargs=[flag for flag in (CPP_FLAGS if self.compiler.compiler_type != "msvc" else [])],
                )
                if objs:
                    glad_obj = objs[0]
                    # print status
                    print(f"\nPrecompiled glad: {glad_obj}")
        except Exception as e:
            print("Warning: failed to precompile glad.c:", e)

        # attach glad object to extensions that request it
        for ext in self.extensions:
            if getattr(ext, "needs_glad", False) and glad_obj:
                extra = list(getattr(ext, "extra_objects", []) or [])
                if glad_obj not in extra:
                    extra.append(glad_obj)
                ext.extra_objects = extra

        super().build_extensions()

# Create Extension objects
extensions = []
mod_map = discover_nanobind_extensions(NANOBIND_SRC_DIR)
for mod_name, sources in mod_map.items():
    ext = Extension(
        mod_name,
        sources=sources,
        include_dirs=INCLUDE_DIRS + [sysconfig.get_paths()["include"]],
        language="c++",
        extra_compile_args=CPP_FLAGS,
        extra_link_args=LINK_ARGS,
    )
    # convention: modules under engine/core/backend/gl3 need glad
    if "engine.core.backend.gl3" in mod_name or "/gl3/" in " ".join(sources):
        ext.needs_glad = True
    else:
        ext.needs_glad = False
    extensions.append(ext)

# If nothing found, keep an empty installable package (no build error).
if not extensions:
    print("No nanobind extensions found under engine/nanobind; setup will install package metadata only.")

setup(
    name="R2D2Engine2D",
    version="1.0.0",
    ext_modules=extensions,
    cmdclass={"build_ext": BuildNoParallel},
)