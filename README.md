# R2D2Engine2D (branche `testing-cmake-migration`)

R2D2Engine2D est en migration vers une architecture native C++23 avec rendu OpenGL 3, fenêtre GLFW native, build CMake, et bindings Python optionnels via nanobind.

## Architecture actuelle

- **Cœur natif :** `engine/core` (C++23 + C/GLAD)
- **Rendu :** OpenGL 3 (`RendererGL3`)
- **Fenêtrage :** GLFW natif (pas de pyglfw)
- **Bindings Python :** module optionnel **`_r2d2`** (nanobind)
- **Build system principal :** CMake
- **Exécutable démo :** `r2d2_demo` (optionnel)
- **Tests :** CTest (`r2d2_performance_test`)

## Dépendances Linux

Ubuntu/Debian :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev python3-dev
```

## Build CMake

Depuis la racine du dépôt :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

Options CMake disponibles :

- `BUILD_PY_BINDINGS` (`ON` par défaut) : construit le module Python `_r2d2`
- `BUILD_DEMO` (`ON` par défaut) : construit `r2d2_demo`

## Exécuter la démo native

```bash
./build/r2d2_demo
```

## Exécuter les tests

```bash
ctest --test-dir build --output-on-failure
```

## API Python réellement exposée (`bindings/bindings.cpp`)

Le module construit est **`_r2d2`** et expose actuellement :

- `_r2d2.Window`
  - constructeur : `Window(width, height, title="R2D2", fullscreen=False, resizable=True)`
  - méthodes : `swap_buffers()`, `poll_events()`, `should_close()`, `run()`, `native_handle()`
- `_r2d2.Lifecycle`
  - méthodes : `init()`, `update()`, `shutdown()`, `is_running()`
- `_r2d2.RendererGL3`
  - méthodes : `init(width, height)`, `begin_frame(r, g, b, a)`, `end_frame()`, `shutdown()`

Note importante : `RendererGL3.init` récupère le handle natif via `engine_window_get_native_handle()` ; la signature Python est donc bien `init(width, height)`.

## Règles threading / OpenGL

- Les appels OpenGL doivent être effectués sur le thread propriétaire du contexte.
- `RendererGL3` stocke un thread propriétaire lors de `init`.
- `begin_frame()` et `end_frame()` retournent sans action si le thread courant n’est pas propriétaire.
- `RendererGL3.end_frame()` effectue déjà le swap (`glfwSwapBuffers`) : éviter un second swap dans la même frame.

## État actuel / roadmap courte

État actuel :

- base CMake + nanobind fonctionnelle
- fenêtre GLFW native
- boucle démo native
- tests CTest de base

Roadmap proche :

1. enrichir l’API native/py (`Texture`, `Mesh`, `Shader`, entrées)
2. renforcer les tests CTest
3. stabiliser l’API publique et la documentation

## Arborescence utile

- `/home/runner/work/R2D2Engine2D/R2D2Engine2D/CMakeLists.txt`
- `/home/runner/work/R2D2Engine2D/R2D2Engine2D/bindings/bindings.cpp`
- `/home/runner/work/R2D2Engine2D/R2D2Engine2D/examples/demo/main.cpp`
- `/home/runner/work/R2D2Engine2D/R2D2Engine2D/tests/performance_profile_test.cpp`
