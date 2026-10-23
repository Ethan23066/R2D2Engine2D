# MIGRATION_CMAKE.md

Ce document décrit l’état **réel** de la branche `testing-cmake-migration`.

## 1) Dépendances Linux

Ubuntu/Debian :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev python3-dev
```

## 2) Configuration / build

Depuis la racine du dépôt :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

Options :

- `BUILD_PY_BINDINGS=ON|OFF`
- `BUILD_DEMO=ON|OFF`

Cibles actuelles :

- `_r2d2` (module Python nanobind, optionnel)
- `r2d2_demo` (démo native, optionnelle)
- `r2d2_performance_test` (test CTest)

> Remarque : il n’existe pas de cible `r2d2_core` dans l’état actuel.

## 3) API bindings exposée

Le module Python est nommé **`_r2d2`** (pas `r2d2`).

Classes exposées (`bindings/bindings.cpp`) :

- `_r2d2.Window`
- `_r2d2.Lifecycle`
- `_r2d2.RendererGL3`

Signature Python actuelle de `RendererGL3.init` :

```python
renderer.init(width, height)
```

(Le handle natif est récupéré en interne via `engine_window_get_native_handle()`.)

## 4) Rendu et swap des buffers

Boucle native de référence (voir `examples/demo/main.cpp`) :

```cpp
while (!engine_window_should_close()) {
    engine_window_poll_events();
    renderer.begin_frame(...);
    renderer.end_frame();
}
```

`RendererGL3.end_frame()` appelle déjà `glfwSwapBuffers(...)`.
Il ne faut donc pas ajouter un second `swap_buffers()` dans la même frame.

## 5) Threading / contexte OpenGL

- Les appels OpenGL doivent rester sur le thread propriétaire du contexte.
- Le propriétaire est fixé pendant `RendererGL3::init(...)`.
- `begin_frame()`/`end_frame()` sortent sans action si le thread courant n’est pas propriétaire.
- Aucune étape `make_current()` n’est exposée dans l’API Python actuelle.

## 6) Exécution

Demo native :

```bash
./build/r2d2_demo
```

Tests :

```bash
ctest --test-dir build --output-on-failure
```
