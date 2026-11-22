#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "engine/core/base/lifecycle.hpp"
#include "engine/core/window/window.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"
#include "engine/core/backend/gl3/ShaderGL3.hpp"
#include "engine/core/backend/gl3/MeshGL3.hpp"
#include "engine/core/backend/gl3/Texture.hpp"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace nb = nanobind;

static r2d2::Lifecycle g_lifecycle;

struct PyWindow {
    std::string py_title;
    void* owned_handle = nullptr;

    PyWindow(int width, int height, const std::string& title = "R2D2",
             bool fullscreen = false, bool resizable = true) {
        if (engine_window_get_native_handle() != nullptr)
            throw std::runtime_error("A native window is already active");

        EngineWindowConfig cfg{};
        cfg.width = width;
        cfg.height = height;
        py_title = title;
        cfg.title = py_title.c_str();
        cfg.fullscreen = fullscreen;
        cfg.resizable = resizable;

        {
            nb::gil_scoped_release release;
            if (!engine_window_init(&cfg))
                throw std::runtime_error("engine_window_init failed");
        }
        owned_handle = engine_window_get_native_handle();
    }

    ~PyWindow() {
        nb::gil_scoped_release release;
        if (owned_handle != nullptr && engine_window_get_native_handle() == owned_handle) {
            engine_window_shutdown();
        }
        owned_handle = nullptr;
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
        nb::gil_scoped_release release;
        return engine_window_should_close();
    }

    void run() {
        nb::gil_scoped_release release;
        engine_window_run();
    }

    void* native_handle() const {
        nb::gil_scoped_release release;
        return engine_window_get_native_handle();
    }

    bool is_key_pressed(int key) const {
        if (!owned_handle || engine_window_get_native_handle() != owned_handle)
            return false;
        nb::gil_scoped_release release;
        return glfwGetKey(static_cast<GLFWwindow*>(owned_handle), key) == GLFW_PRESS;
    }
};

NB_MODULE(_r2d2, m) {
    m.doc() = "R2D2Engine2D native bindings (nanobind Free-Threaded)";

    nb::class_<PyWindow>(m, "Window")
        .def(nb::init<int, int, const std::string&, bool, bool>(),
             nb::arg("width"), nb::arg("height"), nb::arg("title") = "R2D2",
             nb::arg("fullscreen") = false, nb::arg("resizable") = true)
        .def("swap_buffers", &PyWindow::swap_buffers)
        .def("poll_events", &PyWindow::poll_events)
        .def("should_close", &PyWindow::should_close)
        .def("run", &PyWindow::run)
        .def("native_handle", &PyWindow::native_handle)
        .def("is_key_pressed", &PyWindow::is_key_pressed, nb::arg("key"));

    m.attr("KEY_SPACE") = GLFW_KEY_SPACE;
    m.attr("KEY_LEFT")  = GLFW_KEY_LEFT;
    m.attr("KEY_RIGHT") = GLFW_KEY_RIGHT;
    m.attr("KEY_UP")    = GLFW_KEY_UP;
    m.attr("KEY_DOWN")  = GLFW_KEY_DOWN;
    m.attr("KEY_A")     = GLFW_KEY_A;
    m.attr("KEY_D")     = GLFW_KEY_D;
    m.attr("KEY_Q")     = GLFW_KEY_Q;
    m.attr("KEY_W")     = GLFW_KEY_W;
    m.attr("KEY_S")     = GLFW_KEY_S;
    m.attr("KEY_Z")     = GLFW_KEY_Z;
    m.attr("KEY_ESC")   = GLFW_KEY_ESCAPE;

    m.def("init", []() { g_lifecycle.init(); });
    m.def("update", []() { g_lifecycle.update(); });
    m.def("shutdown", []() { g_lifecycle.shutdown(); });
    m.def("is_running", []() { return g_lifecycle.is_running() != 0U; });

    nb::class_<RendererGL3>(m, "RendererGL3")
        .def(nb::init<>())
        .def("init", [](RendererGL3& self, int w, int h) {
            void* handle = engine_window_get_native_handle();
            if (!handle)
                throw std::runtime_error("no native window available on this thread");

            nb::gil_scoped_release release;
            const int result = self.init(handle, w, h);
            if (!result)
                throw std::runtime_error("RendererGL3::init failed");
            return result;
        }, nb::arg("width"), nb::arg("height"))
        .def("begin_frame", &RendererGL3::begin_frame)
        .def("end_frame", &RendererGL3::end_frame)
        .def("shutdown", &RendererGL3::shutdown);

    nb::class_<ShaderGL3>(m, "ShaderGL3")
        .def(nb::init<>())
        .def("load", [](ShaderGL3& self, const std::string& vs_path,
                        const std::string& fs_path) {
            nb::gil_scoped_release release;
            return self.load(vs_path.c_str(), fs_path.c_str());
        }, nb::arg("vs_path"), nb::arg("fs_path"))
        .def("use", &ShaderGL3::use)
        .def("set_int", [](ShaderGL3& self, const std::string& name, int value) {
            self.set_int(name.c_str(), value);
        }, nb::arg("name"), nb::arg("value"))
        .def("set_float", [](ShaderGL3& self, const std::string& name, float value) {
            self.set_float(name.c_str(), value);
        }, nb::arg("name"), nb::arg("value"))
        .def("set_vec2", [](ShaderGL3& self, const std::string& name, float x, float y) {
            self.set_vec2(name.c_str(), x, y);
        }, nb::arg("name"), nb::arg("x"), nb::arg("y"))
        .def("set_vec4", [](ShaderGL3& self, const std::string& name, float x, float y, float z, float w) {
            self.set_vec4(name.c_str(), x, y, z, w);
        }, nb::arg("name"), nb::arg("x"), nb::arg("y"), nb::arg("z"), nb::arg("w"));

    nb::class_<Texture>(m, "Texture")
        .def(nb::init<>())
        .def("load", [](Texture& self, const std::string& path, bool pixelated) {
            nb::gil_scoped_release release;
            return self.load(path.c_str(), pixelated);
        }, nb::arg("path"), nb::arg("pixelated") = true)
        .def("load_from_memory", [](Texture& self, const std::vector<std::uint8_t>& pixels,
                                    int width, int height, bool pixelated) {
            nb::gil_scoped_release release;
            return self.load_from_memory(pixels, width, height, pixelated);
        }, nb::arg("pixels"), nb::arg("width"), nb::arg("height"), nb::arg("pixelated") = true)
        .def("bind", &Texture::bind, nb::arg("unit") = 0)
        .def_prop_ro("width", &Texture::width)
        .def_prop_ro("height", &Texture::height)
        .def_prop_ro("is_loaded", &Texture::is_loaded);

    nb::class_<MeshGL3>(m, "MeshGL3")
        .def(nb::init<>())
        .def("upload", &MeshGL3::upload)
        .def("upload_vertices", [](MeshGL3& self, const std::vector<float>& vertices) {
            return self.upload_vertices(vertices);
        }, nb::arg("vertices"))
        .def("upload_quad", &MeshGL3::upload_quad,
             nb::arg("width") = 1.0f, nb::arg("height") = 1.0f)
        .def("draw", &MeshGL3::draw)
        .def_prop_ro("vertex_count", &MeshGL3::vertex_count);
}
