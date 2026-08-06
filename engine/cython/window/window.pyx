# distutils: language = c++

cdef extern from "window.hpp":
    cdef struct EngineMonitorMode:
        int width
        int height
        int refresh

    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    int engine_window_select_monitor(const EngineMonitorMode* modes, int count) nogil
    int engine_window_init(const EngineWindowConfig* cfg, int monitor_index) nogil
    void engine_window_run() nogil
    void engine_window_shutdown() nogil
    void* engine_window_get_native_handle() nogil

import glfw
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
from libc.stdint cimport uintptr_t

cdef void* NULL = <void*>0

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef int monitor_index
    cdef int mode
    cdef object window

    def __cinit__(self, int width, int height, str title,
                  int monitor_index=0,
                  int mode=0):

        self.cfg.width = width
        self.cfg.height = height
        self.cfg.title = NULL
        self.monitor_index = monitor_index
        self.mode = mode

        cdef bytes _b = title.encode("utf-8")
        cdef const char* _tmp = _b
        self.cfg.title = strdup(_tmp)

        engine_window_init(&self.cfg, self.monitor_index)

        if not glfw.init():
            raise RuntimeError("GLFW init failed")

        self.window = glfw.create_window(width, height, title, None, None)
        if not self.window:
            raise RuntimeError("GLFW window creation failed")

        glfw.make_context_current(self.window)

        monitors = glfw.get_monitors()
        if not monitors:
            return

        if self.monitor_index < 0 or self.monitor_index >= len(monitors):
            self.monitor_index = 0

        monitor = monitors[self.monitor_index]
        mode_info = glfw.get_video_mode(monitor)
        if not mode_info:
            return

        try:
            w = mode_info.width
            h = mode_info.height
        except AttributeError:
            size = getattr(mode_info, 'size', None)
            if size is not None:
                w = getattr(size, 'width', 0)
                h = getattr(size, 'height', 0)
            else:
                w = 0
                h = 0

        r = getattr(mode_info, 'refresh_rate', None)
        if r is None:
            r = getattr(mode_info, 'refreshRate', 0)
        if r is None:
            r = 0

        if self.mode == 1:
            glfw.set_window_monitor(
                self.window,
                monitor,
                0, 0,
                w,
                h,
                r
            )

        elif self.mode == 2:
            glfw.set_window_attrib(self.window, glfw.DECORATED, glfw.FALSE)
            glfw.set_window_attrib(self.window, glfw.RESIZABLE, glfw.FALSE)

            glfw.set_window_monitor(
                self.window,
                None,
                0, 0,
                w,
                h,
                r
            )
            glfw.set_window_pos(self.window, 0, 0)

    @staticmethod
    def list_monitors():
        if not glfw.init():
            return []

        monitors = glfw.get_monitors()
        if not monitors:
            return []

        return [glfw.get_monitor_name(m) for m in monitors]

    @staticmethod
    def get_monitor_mode(int index):
        if not glfw.init():
            return (0, 0, 0)

        monitors = glfw.get_monitors()
        if not monitors or index < 0 or index >= len(monitors):
            return (0, 0, 0)

        mode = glfw.get_video_mode(monitors[index])
        if not mode:
            return (0, 0, 0)

        try:
            w = mode.width
            h = mode.height
        except AttributeError:
            size = getattr(mode, 'size', None)
            if size is not None:
                w = getattr(size, 'width', 0)
                h = getattr(size, 'height', 0)
            else:
                w = 0
                h = 0

        r = getattr(mode, 'refresh_rate', None)
        if r is None:
            r = getattr(mode, 'refreshRate', 0)
        if r is None:
            r = 0

        return (w, h, r)

    @staticmethod
    def select_monitor(modes):
        cdef int count = len(modes)
        if count <= 0:
            return -1

        cdef EngineMonitorMode* arr = <EngineMonitorMode*> malloc(count * sizeof(EngineMonitorMode))
        if arr == NULL:
            raise MemoryError("Failed to allocate monitor array")

        try:
            for i in range(count):
                arr[i].width = modes[i][0]
                arr[i].height = modes[i][1]
                arr[i].refresh = modes[i][2]

            idx = engine_window_select_monitor(arr, count)
        finally:
            free(arr)

        return idx

    def run(self):
        while not glfw.window_should_close(self.window):
            glfw.poll_events()
            glfw.swap_buffers(self.window)

        engine_window_run()

    def shutdown(self):
        try:
            if self.window:
                glfw.destroy_window(self.window)
                self.window = None
        finally:
            glfw.terminate()

        if self.cfg.title != NULL:
            free(<void*> self.cfg.title)
            self.cfg.title = NULL

        engine_window_shutdown()

    def __dealloc__(self):
        if self.cfg.title != NULL:
            try:
                free(<void*> self.cfg.title)
            except Exception:
                pass
            self.cfg.title = NULL

    def get_native_handle(self):
        cdef uintptr_t h = <uintptr_t> engine_window_get_native_handle()
        return h
