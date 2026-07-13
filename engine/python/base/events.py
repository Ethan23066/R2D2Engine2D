from engine.cython.base.events import Events as _Events

class Events:
    def __init__(self):
        self._ev = _Events()

    def push(self, type: int, a: int = 0, b: int = 0):
        self._ev.push(type, a, b)

    def poll(self):
        return self._ev.poll()

    def clear(self):
        self._ev.clear()
