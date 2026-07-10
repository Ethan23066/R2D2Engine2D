from engine.cython.base.config import Config

class PyConfig:
    def __init__(self):
        self._cfg = Config()

    def set_size(self, w: int, h: int):
        self._cfg.set_size(w, h)

    def set_title(self, title: str):
        self._cfg.set_title(title)

    def enable_vsync(self, state: bool):
        self._cfg.enable_vsync(state)

    def fullscreen(self, state: bool):
        self._cfg.fullscreen(state)

    def get(self):
        return self._cfg.get()
