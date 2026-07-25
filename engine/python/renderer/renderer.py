from engine.cython.backend.gl3.gl3 import PyRendererGL3

r = PyRendererGL3()
print(r.init(window_ptr, w, h))
