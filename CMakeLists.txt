cmake_minimum_required(VERSION 3.20)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
include(GNUInstallDirs)

# Build options
option(BUILD_PY_BINDINGS "Build Python bindings (nanobind)" ON)
option(BUILD_DEMO "Build demo executable" ON)

# External deps
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

# Collect engine sources
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.cpp"
)
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.c"
)
list(APPEND ENGINE_C_SRCS "${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/glad.c")

# Python & nanobind
if(BUILD_PY_BINDINGS)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

    include(FetchContent)
    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind
        GIT_TAG v2.7.0
    )
    FetchContent_MakeAvailable(nanobind)

    nanobind_add_module(_r2d2 NB_SHARED
        bindings/bindings.cpp
        ${ENGINE_CPP_SRCS}
        ${ENGINE_C_SRCS}
    )

    target_include_directories(_r2d2 PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/engine/core
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
        ${GLFW_INCLUDEDIR}
        ${Python_INCLUDE_DIRS}
    )

    target_link_libraries(_r2d2 PRIVATE
        ${GLFW_LIBRARIES}
        OpenGL::GL
    )

    set_target_properties(_r2d2 PROPERTIES
        PREFIX ""
        INTERPROCEDURAL_OPTIMIZATION OFF
    )

    install(TARGETS _r2d2
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(BUILD_DEMO)
    set(DEMO_MAIN "${CMAKE_SOURCE_DIR}/examples/demo/main.cpp")
    if(EXISTS "${DEMO_MAIN}")
        add_executable(r2d2_demo
            "${DEMO_MAIN}"
            ${ENGINE_CPP_SRCS}
            ${ENGINE_C_SRCS}
        )
        target_include_directories(r2d2_demo PRIVATE
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/engine/core
            ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3
            ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
            ${GLFW_INCLUDEDIR}
        )
        target_compile_features(r2d2_demo PRIVATE cxx_std_23)
        target_link_libraries(r2d2_demo PRIVATE ${GLFW_LIBRARIES} OpenGL::GL)
    else()
        message(WARNING "BUILD_DEMO enabled but examples/demo/main.cpp not found — skipping demo target")
    endif()
endif()