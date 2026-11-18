# Migration CMake — état de la branche

Ce document décrit la configuration de la branche `testing-cmake-migration`.

## 1. Dépendances Linux

Pour le cœur natif (Debian / Ubuntu) :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev
```

Pour les bindings, installer séparément Python 3.14 Free-Threaded avec ses fichiers de développement. CMake 3.30 ou plus récent est requis pour sélectionner explicitement l'ABI Free-Threaded via `Python_FIND_ABI`.

## 2. Configuration / build

Cœur natif :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=OFF -DBUILD_DEMO=ON
cmake --build build --parallel
```

Avec bindings Python 3.14t :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

Options :

- `BUILD_PY_BINDINGS=ON|OFF`
- `BUILD_DEMO=ON|OFF`
- `BUILD_TESTING=ON|OFF`

## 3. Cibles actuelles

- `r2d2_core` : bibliothèque statique du moteur.
- `_r2d2` : module Python nanobind, si `BUILD_PY_BINDINGS=ON`.
- `r2d2_demo` : démo native si `examples/demo/main.cpp` existe.
- `r2d2_performance_test` : test CTest si `BUILD_TESTING=ON`.

## 4. API et cycle de rendu

Le module Python s'appelle `_r2d2`. Le renderer récupère le handle de la fenêtre via `engine_window_get_native_handle()` et expose :

```python
renderer.init(width, height)
renderer.begin_frame(0.0, 0.0, 0.0, 1.0)
renderer.end_frame()
```

`end_frame()` effectue déjà le swap des buffers : ne pas appeler également `Window.swap_buffers()` pour la même frame.

## 5. Threads et ressources OpenGL

- Les appels GLFW liés à la fenêtre et aux événements doivent rester sur le thread propriétaire.
- Une fermeture demandée depuis un autre thread est mise en attente ; le thread propriétaire la traite au prochain événement.
- Le renderer enregistre le thread et la fenêtre du contexte OpenGL.
- Les ressources shader, mesh et texture refusent les opérations sans le contexte attendu.
- Les ressources doivent être détruites avant `RendererGL3.shutdown()`.

Le mesh ne possède pas encore d'API de données de sommets, et le chargement des textures attend encore un décodeur d'image. Ces fonctionnalités ne sont pas présentées comme opérationnelles.
