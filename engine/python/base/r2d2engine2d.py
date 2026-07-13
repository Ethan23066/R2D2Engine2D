from engine.cython.base.manager import PyRendererManager
from engine.cython.window.engine_window import Window

win = Window(800, 600, "Test GL3")
renderer = PyRendererManager()

renderer.init()

while win.is_open():
    renderer.begin_frame()
    renderer.end_frame()
    win.swap()

renderer.shutdown()
