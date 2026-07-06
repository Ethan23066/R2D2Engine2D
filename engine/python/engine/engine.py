# engine/python/engine/engine.py

from engine.cython.base.engine import PyEngine

class Engine:
    def __init__(self):
        self._engine = PyEngine()

    def init(self):
        self._engine.init()

    def shutdown(self):
        self._engine.shutdown()

    def tick(self):
        self._engine.tick()

    def is_running(self):
        return self._engine.is_running()

    def run(self):
        self.init()
        while self.is_running():
            self.tick()
