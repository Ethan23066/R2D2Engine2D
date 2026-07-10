from engine.cython.base.time import PyTime

_global_time = PyTime()

def update():
    _global_time.update()

def delta():
    return _global_time.delta()

def now():
    return _global_time.now()
