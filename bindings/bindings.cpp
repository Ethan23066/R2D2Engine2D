#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "engine/core/base/lifecycle.hpp"
#include "engine/core/window/window.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"
#include "engine/core/backend/gl3/ShaderGL3.hpp"
#include "engine/core/backend/gl3/MeshGL3.hpp"
#include "engine/core/backend/gl3/Texture.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace nb = nanobind;

// Instance globale statique pour le cycle de vie au niveau du module Python
static r2d2::Lifecycle g_lifecycle;

struct PyWindow {
    std::string py_title;

    PyWindow(int width, int height, const std::string &title = "R2D2", bool fullscreen = false, bool resizable = true) {
        EngineWindowConfig cfg;
        cfg.width = width;
        cfg.height = height;
        py_title = title;
        cfg.title = py_title.c_str();
        cfg.fullscreen = fullscreen;
        cfg.resizable = resizable;

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
        return engine_window_should_close();
    }

    void run() {
        nb::gil_scoped_release release;
        engine_window_run();
    }

    void *native_handle() const {
        return engine_window_get_native_handle();
    }
};

// Déclaration du module avec le flag Free-Threaded
NB_MODULE(_r2d2, m) {
    // Flag indispensable pour nanobind + Python 3.14t (Free-Threaded)
    m.attr("_gil_disabled") = true;
    m.doc() = "R2D2Engine2D native bindings (nanobind Free-Threaded)";

    // --- ENTRÉES ET FENÊTRAGE ---
    nb::class_<PyWindow>(m, "Window")
        .def(nb::init<int, int, const std::string &, bool, bool>(),
             nb::arg("width"), nb::arg("height"), nb::arg("title") = "R2D2",
             nb::arg("fullscreen") = false, nb::arg("resizable") = true,
             "Create a Window (width, height, title, fullscreen, resizable)")
        .def("swap_buffers", &PyWindow::swap_buffers)
        .def("poll_events", &PyWindow::poll_events)
        .def("should_close", &PyWindow::should_close)
        .def("run", &PyWindow::run)
        .def("native_handle", &PyWindow::native_handle);

    // --- REFAÇONNAGE DE LIFECYCLE (Fonctions globales de module) ---
    m.def("init", []() { g_lifecycle.init(); }, "Initialize the engine lifecycle");
    m.def("update", []() { g_lifecycle.update(); }, "Update the engine lifecycle");
    m.def("shutdown", []() { g_lifecycle.shutdown(); }, "Shutdown the engine lifecycle");
    m.def("is_running", []() { return g_lifecycle.is_running() != 0U; }, "Check if the engine is running");

    // --- RENDU OPENGL 3 ---
    nb::class_<RendererGL3>(m, "RendererGL3")
        .def(nb::init<>())
        .def("init", [](RendererGL3 &self, int w, int h) {
            void *handle = engine_window_get_native_handle();
            if (!handle)
                throw std::runtime_error("no native window available");
            nb::gil_scoped_release release;
            int res = self.init(handle, w, h);
            if (!res)
                throw std::runtime_error("RendererGL3::init failed");
            return res;
        }, nb::arg("width"), nb::arg("height"), "Initialize renderer with current window and size")
        .def("begin_frame", &RendererGL3::begin_frame,
             nb::arg("r"), nb::arg("g"), nb::arg("b"), nb::arg("a"))
        .def("end_frame", &RendererGL3::end_frame)
        .def("shutdown", &RendererGL3::shutdown);

    // --- SHADER GL3 ---
    nb::class_<ShaderGL3>(m, "ShaderGL3")
        .def(nb::init<>())
        .def("load", [](ShaderGL3 &self, const std::string &vs_path, const std::string &fs_path) {
            nb::gil_scoped_release release;
            return self.load(vs_path.c_str(), fs_path.c_str());
        }, nb::arg("vs_path"), nb::arg("fs_path"), "Load and compile shaders from file paths")
        .def("use", [](ShaderGL3 &self) {
            self.use();
        }, "Bind the shader program for rendering");

    // --- TEXTURE ---
    nb::class_<Texture>(m, "Texture")
        .def(nb::init<>())
        .def("load", [](Texture &self, const std::string &path) {
            nb::gil_scoped_release release;
            return self.load(path.c_str());
        }, nb::arg("path"), "Load an image file into an OpenGL texture")
        .def("bind", [](Texture &self, int unit) {
            self.bind(unit);
        }, nb::arg("unit"), "Bind texture to a specific texture unit slot");

    // --- MESH GL3 ---
    nb::class_<MeshGL3>(m, "MeshGL3")
        .def(nb::init<>())
        .def("upload", [](MeshGL3 &self) {
            self.upload();
        }, "Upload mesh geometry data to the GPU buffers")
        .def("draw", [](MeshGL3 &self) {
            self.draw();
        }, "Submit mesh geometry to the current GPU bound pipeline");
}
