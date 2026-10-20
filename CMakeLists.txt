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

# Python & nanobind (only if requested)
if(BUILD_PY_BINDINGS)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

    include(FetchContent)
    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind
        GIT_TAG v2.7.0
    )
    FetchContent_MakeAvailable(nanobind)

    if(TARGET nanobind::nanobind)
        set(NANOBIND_TARGET nanobind::nanobind)
        message(STATUS "Using nanobind target: nanobind::nanobind")
    elseif(TARGET nanobind)
        set(NANOBIND_TARGET nanobind)
        message(STATUS "Using nanobind target: nanobind")
    else()
        add_library(nanobind::nanobind INTERFACE IMPORTED)
        if(DEFINED nanobind_SOURCE_DIR)
            set_target_properties(nanobind::nanobind PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${nanobind_SOURCE_DIR}/include"
            )
            message(STATUS "Created fallback nanobind::nanobind INTERFACE target -> ${nanobind_SOURCE_DIR}/include")
        else()
            set_target_properties(nanobind::nanobind PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/third_party/nanobind/include"
            )
            message(WARNING "nanobind source dir unknown — created fallback INTERFACE target pointing to ${CMAKE_SOURCE_DIR}/third_party/nanobind/include")
        endif()
        set(NANOBIND_TARGET nanobind::nanobind)
    endif()
endif()

# Find GLFW (pkg-config) and OpenGL
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

# Collect engine sources (explicitly include glad.c as C source)
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.cpp"
)
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.c"
)
list(APPEND ENGINE_C_SRCS "${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/glad.c")

if(BUILD_PY_BINDINGS)
    add_library(_r2d2 MODULE
        bindings/bindings.cpp
        ${ENGINE_CPP_SRCS}
        ${ENGINE_C_SRCS}
    )

    set(R2D2_PUBLIC_INCLUDES
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/engine/core
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
        ${GLFW_INCLUDEDIR}
    )

    if(DEFINED nanobind_SOURCE_DIR)
        list(APPEND R2D2_PUBLIC_INCLUDES "${nanobind_SOURCE_DIR}/include")
    endif()

    if(DEFINED Python_INCLUDE_DIRS)
        list(APPEND R2D2_PUBLIC_INCLUDES "${Python_INCLUDE_DIRS}")
    endif()

    target_include_directories(_r2d2 PRIVATE ${R2D2_PUBLIC_INCLUDES})
    target_compile_features(_r2d2 PRIVATE cxx_std_23)

    # Disable LTO for Python extension modules: it can break relocation/ABI on free-threaded builds.
    target_compile_options(_r2d2 PRIVATE -fno-lto)
    target_link_options(_r2d2 PRIVATE -fno-lto)

    # Avoid linking libpython statically into a Python extension module; nanobind handles the ABI.
    target_link_libraries(_r2d2 PRIVATE
        Python::Module
        ${NANOBIND_TARGET}
        ${GLFW_LIBRARIES}
        OpenGL::GL
    )

    set_target_properties(_r2d2 PROPERTIES
        PREFIX ""
        INTERPROCEDURAL_OPTIMIZATION OFF
    )

    install(TARGETS _r2d2
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
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
