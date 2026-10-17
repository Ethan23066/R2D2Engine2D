cmake_minimum_required(VERSION 3.20)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Python
find_package(Python COMPONENTS Development REQUIRED)

# Find GLFW (pkg-config) and OpenGL
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

# Fetch nanobind
include(FetchContent)
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind
  GIT_TAG        master
)
FetchContent_MakeAvailable(nanobind)

# Collect engine sources (explicitly include glad.c as C source)
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/engine/core/*.cpp"
)
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/engine/core/*.c"
)
list(APPEND ENGINE_C_SRCS "${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/glad.c")

add_library(_r2d2 MODULE
    bindings/bindings.cpp
    ${ENGINE_CPP_SRCS}
    ${ENGINE_C_SRCS}
)

target_include_directories(_r2d2 PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/core
    ${GLFW_INCLUDEDIR}
)

target_compile_features(_r2d2 PRIVATE cxx_std_23)

# Link libraries: nanobind, Python, GLFW, OpenGL
target_link_libraries(_r2d2 PRIVATE nanobind::nanobind Python::Python ${GLFW_LIBRARIES} OpenGL::GL)

# Ensure Python can import the extension as r2d2._r2d2
set_target_properties(_r2d2 PROPERTIES PREFIX "")

# Install rule (optional) - place in Python package layout if using install
install(TARGETS _r2d2
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
