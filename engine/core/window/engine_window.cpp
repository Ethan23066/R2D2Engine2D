#include "engine_window.hpp"

int engine_window_init(EngineWindowConfig cfg) {
    return 1; // succès
}


void engine_window_run() {
    // rien ici → la boucle est en Python via pyglfw
}

void engine_window_shutdown() {
    // rien ici → pyglfw gère la destruction
}
