from engine.cython.backend.manager import BackendManager
from engine.python.window.engine_window import Window

win = Window(800, 600, "Test GL3")

renderer = BackendManager()
renderer.init(800, 600)

while win.is_open():
    renderer.begin_frame(0.1, 0.1, 0.1, 1.0)
    renderer.end_frame()
    win.swap()

renderer.shutdown()
