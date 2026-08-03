from engine.cython.base.time import PyTime

class Time:
    def __init__(self):
        self._t = PyTime()

    def update(self):
        self._t.update()

    def delta(self):
        return self._t.delta()

    def now(self):
        return self._t.now()
