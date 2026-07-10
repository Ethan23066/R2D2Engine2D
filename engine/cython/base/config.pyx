# distutils: language = c++

from .config cimport EngineConfig, load_default_config
from libcpp.string cimport string

cdef class Config:
    cdef EngineConfig cfg

    def __cinit__(self):
        load_default_config(self.cfg)

    def set_size(self, int w, int h):
        self.cfg.window_width = w
        self.cfg.window_height = h

    def set_title(self, str t):
        self.cfg.title = string(t.encode("utf-8"))

    def enable_vsync(self, bint state):
        self.cfg.vsync = state

    def fullscreen(self, bint state):
        self.cfg.fullscreen = state

    def get(self):
        return {
            "width": self.cfg.window_width,
            "height": self.cfg.window_height,
            "vsync": bool(self.cfg.vsync),
            "fullscreen": bool(self.cfg.fullscreen),
            "title": self.cfg.title.decode("utf-8")
        }
