# R2D2Engine2D

R2D2Engine2D est un moteur 2D en cours de développement. Cette branche prépare un cœur natif en C++23, un backend graphique OpenGL, le fenêtrage GLFW et une API Python optionnelle basée sur nanobind.

Le projet évolue encore : certaines parties sont des prototypes et toutes les fonctionnalités annoncées dans l'architecture ne sont pas encore implémentées. Cette documentation distingue les éléments présents dans le code de ceux qui restent à développer.

## État du projet

| Partie | État |
|---|---|
| Cœur natif C++23 | En développement |
| Configuration et compilation CMake | Présentes |
| Fenêtrage GLFW | Présent |
| Backend OpenGL 3.1 | Prototype en développement |
| Bindings Python avec nanobind | Optionnels, ABI Free-Threaded |
| Démo native | Optionnelle, selon la présence de sa source |
| Tests CTest | Profil de performances et file de tâches |

## Organisation du dépôt

- `engine/core/` : code du moteur natif.
- `engine/core/backend/gl3/` : backend graphique OpenGL.
- `engine/core/window/` : gestion des fenêtres GLFW.
- `bindings/` : bindings Python.
- `examples/` : exemples et démo native.
- `tests/` : tests du projet.

## Prérequis

Pour le cœur natif :

- un compilateur C prenant en charge C23 ;
- un compilateur C++ prenant en charge C++23 ;
- CMake 3.21 ou plus récent ;
- pkg-config ;
- GLFW 3 ;
- les bibliothèques de développement OpenGL ;
- les bibliothèques de threads du système.

Pour les bindings Python :

- CMake 3.30 ou plus récent (détection explicite de l'ABI Free-Threaded) ;
- Python 3.14 Free-Threaded et ses fichiers de développement ;
- nanobind 3.1.0, récupéré à une version Git précise par CMake.

### Debian / Ubuntu

Pour le cœur natif :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev
```

Python 3.14t doit être installé séparément avec ses fichiers de développement. Les paquets disponibles dépendent de la distribution et de la méthode d'installation de Python.

## Compilation

### Cœur natif et démo, sans bindings Python

Cette configuration ne recherche pas Python et peut être utilisée sur les machines qui ne disposent pas de Python Free-Threaded :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=OFF -DBUILD_DEMO=ON
cmake --build build --parallel
```

### Avec les bindings Python Free-Threaded

Avec CMake 3.30+ et Python 3.14t :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

### Options CMake

| Option | Valeur par défaut | Description |
|---|---:|---|
| `BUILD_PY_BINDINGS` | `ON` | Construit le module Python `_r2d2`. |
| `BUILD_DEMO` | `ON` | Construit `r2d2_demo` si `examples/demo/main.cpp` existe. |
| `BUILD_TESTING` | `ON` | Active les tests CTest. |

## Exécution

Si la cible de démo a été construite :

```bash
./build/r2d2_demo
```

Si les bindings Python ont été construits et que le module est dans le chemin d'import Python :

```python
import _r2d2
```

## Tests

Les tests restent actifs en configuration Release : ils utilisent des vérifications explicites et non `assert()`, qui peut être désactivé par `NDEBUG`.

```bash
ctest --test-dir build --output-on-failure
```

## API Python actuellement déclarée

Le module `_r2d2` est défini dans `bindings/bindings.cpp). Il expose actuellement :

- `Window(width, height, title="R2D2", fullscreen=False, resizable=True)`
  - `swap_buffers()`
  - `poll_events()`
  - `should_close()`
  - `run()`
  - `native_handle()`
- Fonctions de cycle de vie :
  - `init()`
  - `update()`
  - `shutdown()`
  - `is_running()`
- `RendererGL3()`
  - `init(width, height)`
  - `begin_frame(r, g, b, a)`
  - `end_frame()`
  - `shutdown()`
- `ShaderGL3()`
  - `load(vs_path, fs_path)`
  - `use()`
- `Texture()`
  - `load(path)`
  - `bind(unit)`
- `MeshGL3()`
  - `upload()`
  - `draw()`

Cette liste décrit les interfaces déclarées, pas une garantie que chaque fonctionnalité soit déjà complète.

## État des fonctionnalités graphiques

Le backend OpenGL est en cours de construction :

- Le renderer initialise le contexte et gère le début et la fin d'une frame.
- Les objets shader vérifient la compilation et le linkage des programmes.
- Le mesh actuel alloue un VAO et un VBO, mais ne dispose pas encore d'API pour envoyer des sommets ; son appel `draw()` ne dessine donc rien.
- Le chargement des textures n'est pas encore implémenté. En l'absence de décodeur d'image, `Texture.load()` retourne `False`.

Les ressources OpenGL doivent être détruites avant l'arrêt du renderer et pendant que le contexte propriétaire est encore courant. Les opérations GLFW de fenêtre et d'événements restent confinées au thread qui a créé la fenêtre. Une demande de fermeture provenant d'un autre thread est différée jusqu'au prochain passage du thread propriétaire dans la boucle d'événements.

## Threads et contexte OpenGL

La file de tâches est un outil de synchronisation CPU ; elle ne rend pas les appels OpenGL sûrs depuis des workers. Un contexte OpenGL ne doit être utilisé que sur le thread qui le possède.

Le projet ne fixe pas de nombre universel de threads pour les jeux. Le nombre de workers et leur répartition doivent rester proportionnés aux besoins et aux mesures, notamment sur les petites configurations.

## Forks et modifications personnelles

Le dépôt officiel n'accepte pas de contributions externes ni de pull requests.

Toute personne souhaitant modifier le moteur ou expérimenter une autre direction doit d'abord créer son propre fork. Les modifications et évolutions peuvent ensuite être réalisées librement dans ce fork, indépendamment du dépôt officiel.
