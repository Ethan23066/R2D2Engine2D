# R2D2Engine2D

R2D2Engine2D est un moteur 2D en cours de développement. Cette branche correspond à la migration vers un cœur natif en C++23, avec un backend graphique OpenGL, une gestion des fenêtres via GLFW et une API Python optionnelle basée sur nanobind.

Le projet évolue encore : certaines parties sont des prototypes et toutes les fonctionnalités annoncées dans l'architecture ne sont pas encore implémentées. Cette documentation distingue donc les éléments présents dans le code de ceux qui restent à développer.

## État du projet

| Partie | État |
|---|---|
| Cœur natif C++23 | En développement |
| Configuration et compilation CMake | Présentes |
| Fenêtrage GLFW | Présent |
| Backend OpenGL | En développement |
| Bindings Python avec nanobind | Optionnels |
| Démo native | Optionnelle, selon la présence de sa source |
| Tests CTest | Un test de profil de performance est déclaré |

## Organisation du dépôt

- `engine/core/` : code du moteur natif.
- `engine/core/backend/gl3/` : backend graphique OpenGL.
- `engine/core/window/` : gestion des fenêtres GLFW.
- `bindings/` : bindings Python.
- `examples/` : exemples et démo native.
- `tests/` : tests du projet.

## Prérequis

La configuration CMake utilise :

- un compilateur C et C++ prenant en charge C23 et C++23 ;
- CMake 3.20 ou plus récent ;
- pkg-config ;
- GLFW 3 ;
- les bibliothèques de développement OpenGL ;
- les bibliothèques de threads du système.

### Debian / Ubuntu

Pour compiler le cœur natif et la démo :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev
```

Pour construire également les bindings Python, cette branche configure nanobind avec l'ABI Free-Threaded de Python 3.14. Il faut donc disposer d'une installation Python 3.14 Free-Threaded compatible, avec ses fichiers de développement. La configuration récupère nanobind depuis son dépôt Git lors du build.

Les noms des paquets Python et leur disponibilité dépendent de la distribution et de la façon dont Python 3.14t a été installé.

## Compilation

### Cœur natif et démo, sans bindings Python

Cette option permet de compiler le moteur sans dépendre de l'installation Python Free-Threaded :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=OFF -DBUILD_DEMO=ON
cmake --build build --parallel
```

### Avec les bindings Python

Si Python 3.14 Free-Threaded et ses fichiers de développement sont installés :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

### Options CMake

| Option | Valeur par défaut | Description |
|---|---:|---|
| `BUILD_PY_BINDINGS` | `ON` | Construit le module Python `_r2d2`. |
| `BUILD_DEMO` | `ON` | Construit `r2d2_demo` si `examples/demo/main.cpp` existe. |

Le test de performance est déclaré séparément et est ajouté à CTest.

## Exécution

Si la cible de démo a été construite :

```bash
./build/r2d2_demo
```

Si les bindings Python ont été construits, le module `_r2d2` doit être accessible depuis l'environnement Python utilisé :

```python
import _r2d2
```

## Tests

```bash
ctest --test-dir build --output-on-failure
```

Le résultat des tests dépend des cibles effectivement configurées et compilées.

## API Python actuellement déclarée

Le module `_r2d2` est défini dans `bindings/bindings.cpp`. Il expose actuellement :

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
- Le chargement des shaders lit les fichiers, compile les deux étapes et vérifie leur linkage.
- Le mesh actuel est un prototype : il crée ses objets OpenGL, mais ne reçoit pas encore de données de sommets.
- Le chargement des textures n'est pas encore implémenté. En l'absence de décodeur d'image, `Texture.load()` retourne actuellement `False`.

Les ressources OpenGL doivent être utilisées et détruites pendant que leur contexte est valide et sur le thread qui possède ce contexte.

## Threads et contexte OpenGL

Le projet peut évoluer vers différentes organisations de threads selon les besoins du moteur et des applications qui l'utilisent. Cette branche ne fixe pas de nombre universel de threads pour tous les jeux.

Une contrainte vient toutefois d'OpenGL : un contexte doit être courant sur le thread qui effectue les appels graphiques. Les appels GLFW liés à la fenêtre et aux événements doivent également respecter les règles de thread de GLFW.

Le nombre de workers, leur répartition et les stratégies de parallélisation restent des choix d'architecture susceptibles d'évoluer avec les mesures et les besoins.

## Forks et modifications personnelles

Le dépôt officiel n'accepte pas de contributions externes ni de pull requests.

Toute personne souhaitant modifier le moteur ou expérimenter une autre direction doit d'abord créer son propre fork. Les modifications et évolutions peuvent ensuite être réalisées librement dans ce fork, indépendamment du dépôt officiel.

Cette règle permet de conserver une direction cohérente pour le projet officiel, sans empêcher les autres développeurs d'adapter le moteur à leurs besoins.
