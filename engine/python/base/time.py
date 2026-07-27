# python/base/r2d2core.py

import glfw
from engine.cython.window.engine_window import EngineWindow
from engine.cython.backend.manager import BackendManager


class R2D2Core:
    def __init__(self, width: int = 800, height: int = 600, title: str = "R2D2Core"):
        self.width = width
        self.height = height
        self.title = title

        # Fenêtre PyGLFW via Cython
        self.window = EngineWindow(width, height, title)

        # Backend GL3 via Cython → C++
        self.backend = BackendManager()

    def init(self) -> bool:
        # Création de la fenêtre
        self.window.init()

        # Initialisation du backend GL3
        ok = self.backend.init(self.window.ptr(), self.width, self.height)
        return bool(ok)

    def run(self):
        # Boucle principale
        while not glfw.window_should_close(self.window.py_window):
            self.backend.begin_frame()
            glfw.swap_buffers(self.window.py_window)
            glfw.poll_events()

        self.shutdown()

    def shutdown(self):
        self.backend.shutdown()
        glfw.terminate()


# Mode standalone
if __name__ == "__main__":
    core = R2D2Core(800, 600, "R2D2Core")
    if core.init():
        core.run()
