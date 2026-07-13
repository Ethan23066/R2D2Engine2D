from engine.cython.base.lifecycle import PyLifecycle

class Lifecycle:
    def __init__(self):
        self._lc = PyLifecycle()

    def init(self):
        self._lc.init()

    def update(self):
        self._lc.update()

    def shutdown(self):
        self._lc.shutdown()

    def is_running(self):
        return self._lc.is_running()
