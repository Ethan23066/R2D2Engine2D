# R2D2Engine2D

R2D2Engine2D est un moteur de jeu en cours de développement. Cette branche utilise un cœur natif en C++23, GLFW pour la création des fenêtres et la gestion des événements, ainsi qu'un backend graphique OpenGL 3.1. Une API Python optionnelle expose une partie du moteur via nanobind.

Le projet n'est pas encore une version stable ni un moteur complet. Ce README décrit les fonctionnalités présentes dans cette branche et distingue les composants utilisables des parties encore limitées.

## État du projet

| Composant | État actuel |
|---|---|
| Cœur natif C++23 | En développement |
| Configuration et compilation CMake | En place |
| Fenêtres et événements GLFW | Implémentés |
| Backend OpenGL 3.1 | Premières fonctions opérationnelles, API encore en développement |
| Shaders | Compilation, linkage et uniformes implémentés |
| Textures | Chargement d'images et upload RGBA implémentés avec stb_image |
| Meshes | Upload de sommets et dessin de triangles implémentés |
| Bindings Python | Optionnels, via nanobind |
| Démo native | Présente |
| Tests CTest | Profil de frame, file de tâches et sélection du moniteur |

## Organisation du dépôt

- `CMakeLists.txt` : configuration du projet et de ses cibles.
- `engine/core/base/` : cycle de vie, configuration, événements, journalisation, système et temps.
- `engine/core/backend/gl3/` : renderer OpenGL, shaders, textures, meshes, gestion du contexte et GLAD.
- `engine/core/window/` : fenêtre GLFW et gestion de son cycle de vie.
- `engine/core/threads/` : file de tâches thread-safe.
- `engine/core/performance/` : outils de mesure du temps de frame.
- `bindings/` : module Python nanobind `_r2d2`.
- `examples/demo/` : démonstration native.
- `tests/` : tests du projet.

## Prérequis

### Cœur natif

- Un compilateur C prenant en charge C23.
- Un compilateur C++ prenant en charge C++23.
- CMake 3.21 ou plus récent.
- pkg-config.
- GLFW 3 et ses fichiers de développement.
- Les bibliothèques de développement OpenGL.
- Les bibliothèques de threads du système.
- Git et un accès réseau lors de la configuration CMake, pour récupérer les dépendances FetchContent.

CMake récupère automatiquement :
- **stb** depuis son dépôt Git, pour le chargement des images ;
- **nanobind v3.1.0**, uniquement lorsque les bindings Python sont activés.

### Bindings Python (facultatifs)

- Python 3.12 ou plus récent.
- Les fichiers de développement correspondant à l'interpréteur Python utilisé.
- nanobind, récupéré automatiquement par CMake.

Le projet détecte si l'interpréteur Python sélectionné est Free-Threaded (`Py_GIL_DISABLED`). Dans ce cas, le module nanobind est construit avec le support Free-Threaded. Sinon, il est construit avec l'ABI Python classique et le GIL. Pour utiliser une installation Free-Threaded, il faut donc sélectionner cet interpréteur lors de la configuration CMake.

## Installation des dépendances (Debian / Ubuntu)

Pour le cœur natif :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libglfw3-dev libgl1-mesa-dev git
```

Pour les bindings Python, installe également les fichiers de développement correspondant à Python 3.12 ou plus récent. Le nom du paquet dépend de la version de Python fournie par ta distribution (par exemple `python3-dev` lorsque cette version convient).

## Compilation

Les commandes suivantes sont à exécuter depuis la racine du dépôt.

### Cœur natif, sans Python

Cette configuration ne recherche pas Python et construit le cœur natif ainsi que la démo :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=OFF -DBUILD_DEMO=ON
cmake --build build --parallel
```

### Avec les bindings Python

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DBUILD_DEMO=ON
cmake --build build --parallel
```

Pour demander explicitement à CMake d'utiliser un interpréteur Free-Threaded, indique son chemin lors de la configuration (ainsi que ses fichiers de développement) :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON -DPython_EXECUTABLE=/chemin/vers/python3.13t
cmake --build build --parallel
```

Adapte le chemin à l'installation présente sur ta machine. Si l'interpréteur sélectionné n'est pas Free-Threaded, CMake construit le module avec le GIL classique.

### Options CMake

| Option | Valeur par défaut | Description |
|---|---:|---|
| `BUILD_PY_BINDINGS` | `ON` | Construit le module Python natif `_r2d2`. |
| `BUILD_DEMO` | `ON` | Construit `r2d2_demo` si `examples/demo/main.cpp` est présent. |
| `BUILD_TESTING` | `ON` | Active la cible de test CTest `r2d2_performance_test`. |

Le cœur natif `r2d2_core` est construit dans les deux configurations. Les dépendances FetchContent nécessitent un accès réseau lors de leur première récupération.

## Exécution

### Démo native

Après une compilation avec `BUILD_DEMO=ON` :

```bash
./build/r2d2_demo
```

La démo crée une fenêtre GLFW et utilise le renderer pour effacer l'écran et présenter les frames.

### Module Python

Après une compilation avec `BUILD_PY_BINDINGS=ON`, le module `_r2d2` doit être accessible depuis le chemin d'import de Python :

```python
import _r2d2
```

Le module est une extension native : il ne s'agit pas d'un package Python autonome installé automatiquement dans un environnement virtuel.

## Tests

Les tests sont activés par défaut avec CTest. Ils vérifient notamment :
- le calcul des mesures de temps de frame et des valeurs recommandées ;
- les opérations de base de `ThreadSafeQueue`, sa fermeture, sa réinitialisation et sa capacité maximale ;
- un échange producteur / consommateur entre threads ;
- la sélection d'un mode de moniteur.

Les vérifications utilisent une macro `CHECK` explicite plutôt que `assert()`, afin de rester actives en configuration Release.

```bash
ctest --test-dir build --output-on-failure
```

## API Python actuellement exposée

Les bindings sont définis dans `bindings/bindings.cpp`. Les interfaces exposées dans cette branche sont :

### Fenêtre

```python
window = _r2d2.Window(width, height, title="R2D2", fullscreen=False, resizable=True)
```

Méthodes :
- `swap_buffers()`
- `poll_events()`
- `should_close()`
- `run()`
- `native_handle()`
- `is_key_pressed(key)`

Quelques constantes de touches sont également exposées, notamment `KEY_SPACE`, `KEY_LEFT`, `KEY_RIGHT`, `KEY_UP`, `KEY_DOWN`, `KEY_A`, `KEY_D`, `KEY_Q`, `KEY_W`, `KEY_S`, `KEY_Z` et `KEY_ESC`.

### Cycle de vie

- `init()`
- `update()`
- `shutdown()`
- `is_running()`

À ce stade, `update()` est encore un point d'extension : les mises à jour du monde, de l'ECS et de la physique sont indiquées comme travaux futurs dans le code.

### Renderer OpenGL

- `RendererGL3()`
- `init(width, height)`
- `begin_frame(r, g, b, a)`
- `end_frame()`
- `shutdown()`

Le renderer utilise la fenêtre native déjà créée sur le thread courant.

### Shaders

- `ShaderGL3()`
- `load(vs_path, fs_path)`
- `use()`
- `set_int(name, value)`
- `set_float(name, value)`
- `set_vec2(name, x, y)`
- `set_vec4(name, x, y, z, w)`

### Textures

- `Texture()`
- `load(path, pixelated=True)`
- `load_from_memory(pixels, width, height, pixelated=True)`
- `bind(unit=0)`
- Propriétés en lecture seule : `width`, `height`, `is_loaded`

Le chargement depuis un fichier utilise stb_image. Les données envoyées à `load_from_memory` doivent être des pixels RGBA.

### Meshes

- `MeshGL3()`
- `upload()`
- `upload_vertices(vertices)`
- `upload_quad(width=1.0, height=1.0)`
- `draw()`
- Propriété en lecture seule : `vertex_count`

Chaque sommet transmis à `upload_vertices` est représenté par cinq valeurs flottantes : position `(x, y, z)`, puis coordonnées de texture `(u, v)`.

Cette liste décrit les interfaces effectivement liées par nanobind ; elle ne signifie pas que le moteur dispose déjà de toutes les fonctions attendues d'un moteur de jeu complet.

## Fenêtres, threads et contexte OpenGL

GLFW impose que les opérations sur une fenêtre soient effectuées par le thread qui l'a créée. Le moteur conserve cette règle :
- la création et la destruction de la fenêtre restent liées à son thread propriétaire ;
- `poll_events()`, `swap_buffers()` et la boucle `run()` sont destinés à être utilisés depuis ce thread ;
- une demande de fermeture provenant d'un autre thread est signalée au thread propriétaire, qui effectue la destruction lors du traitement suivant des événements.

Le contexte OpenGL est lui aussi associé à un thread propriétaire. Les appels de rendu, le chargement des shaders, l'upload des textures et des meshes ainsi que la destruction des ressources OpenGL doivent être effectués avec le contexte approprié courant sur ce thread.

Il faut libérer les ressources OpenGL avant d'appeler `RendererGL3.shutdown()` et avant de détruire la fenêtre. La file `ThreadSafeQueue` fournit une synchronisation côté CPU ; elle ne rend pas les appels OpenGL sûrs depuis des workers.

Le projet ne fixe pas de nombre universel de threads pour les jeux. Le nombre de workers doit dépendre des tâches réelles et des mesures effectuées sur les configurations visées.

## Forks et modifications personnelles

Le dépôt officiel n'accepte pas de contributions externes ni de pull requests.

Toute personne souhaitant modifier le moteur ou expérimenter une autre direction doit d'abord créer son propre fork. Les modifications et évolutions peuvent ensuite être réalisées librement dans ce fork, indépendamment du dépôt officiel.
