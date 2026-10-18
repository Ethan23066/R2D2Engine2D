#include <nanobind/nanobind.h>
#include <nanobind/stl.h>

#include "engine/core/window/window.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"

#include <stdexcept>
#include <string>

namespace nb = nanobind;

struct PyWindow {
    std::string py_title;

    PyWindow(int width, int height, const std::string &title = "R2D2", bool fullscreen = false, bool resizable = true) {
        EngineWindowConfig cfg;
        cfg.width = width;
        cfg.height = height;
        py_title = title;                     // keep ownership of the string
        cfg.title = py_title.c_str();
        cfg.fullscreen = fullscreen;
        cfg.resizable = resizable;

        // engine_window_init may call into native libraries; release the GIL while doing native work
        nb::gil_scoped_release release;
        if (!engine_window_init(&cfg))
            throw std::runtime_error("engine_window_init failed");
    }

    ~PyWindow() {
        nb::gil_scoped_release release;
        engine_window_shutdown();
    }

    void swap_buffers() {
        nb::gil_scoped_release release;
        engine_window_swap_buffers();
    }

    void poll_events() {
        nb::gil_scoped_release release;
        engine_window_poll_events();
    }

    bool should_close() const {
        // polling state is cheap; keep GIL for simple boolean check
        return engine_window_should_close();
    }

    void run() {
        // run is a potentially long blocking call -> release GIL so Python threads can run
        nb::gil_scoped_release release;
        engine_window_run();
    }

    void *native_handle() const {
        return engine_window_get_native_handle();
    }
};

NB_MODULE(_r2d2, m) {
    m.doc() = "R2D2Engine2D native bindings (nanobind)";

    nb::class_<PyWindow>(m, "Window")
        .def(nb::init<int, int, const std::string &, bool, bool>(),
             "Create a Window (width, height, title, fullscreen, resizable)")
        .def("swap_buffers", &PyWindow::swap_buffers)
        .def("poll_events", &PyWindow::poll_events)
        .def("should_close", &PyWindow::should_close)
        .def("run", &PyWindow::run)
        .def("native_handle", &PyWindow::native_handle);

    nb::class_<RendererGL3>(m, "RendererGL3")
        .def(nb::init<>())
        .def("init", [](RendererGL3 &self, int w, int h) {
            void *handle = engine_window_get_native_handle();
            if (!handle)
                throw std::runtime_error("no native window available");
            // Loading GL function pointers and doing GL setup can be slow; run without GIL
            nb::gil_scoped_release release;
            int res = self.init(handle, w, h);
            if (!res)
                throw std::runtime_error("RendererGL3::init failed");
            return res;
        }, "Initialize renderer with current window and size")
        .def("begin_frame", &RendererGL3::begin_frame)
        .def("end_frame", &RendererGL3::end_frame)
        .def("shutdown", &RendererGL3::shutdown);
}
