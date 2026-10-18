cmake_minimum_required(VERSION 3.20)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(GNUInstallDirs)

# Build options
option(BUILD_PY_BINDINGS "Build Python bindings (nanobind)" ON)
option(BUILD_DEMO "Build demo executable" ON)

# Python & nanobind (only if requested)
if(BUILD_PY_BINDINGS)
    find_package(Python COMPONENTS Interpreter Development REQUIRED)

    include(FetchContent)
    FetchContent_Declare(
      nanobind
      GIT_REPOSITORY https://github.com/wjakob/nanobind
      GIT_TAG        master
    )
    FetchContent_MakeAvailable(nanobind)

    # Prefer the official target if provided, otherwise create a minimal INTERFACE target
    if(TARGET nanobind::nanobind)
        set(NANOBIND_TARGET nanobind::nanobind)
        message(STATUS "Using nanobind target: nanobind::nanobind")
    elseif(TARGET nanobind)
        set(NANOBIND_TARGET nanobind)
        message(STATUS "Using nanobind target: nanobind")
    else()
        # Create an imported INTERFACE target that points to the fetched nanobind headers.
        # This fallback is intentionally minimal: it only provides include paths so C++
        # compilation can find <nanobind/nanobind.h>. If nanobind provides a full CMake
        # target in the future, that will be preferred.
        add_library(nanobind::nanobind INTERFACE IMPORTED)
        if(DEFINED nanobind_SOURCE_DIR)
            set_target_properties(nanobind::nanobind PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${nanobind_SOURCE_DIR}/include"
            )
            message(STATUS "Created fallback nanobind::nanobind INTERFACE target -> ${nanobind_SOURCE_DIR}/include")
        else()
            # As a last resort, try a common location relative to the source tree
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

    target_include_directories(_r2d2 PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/engine/core
        ${GLFW_INCLUDEDIR}
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/include
    )

    target_compile_features(_r2d2 PRIVATE cxx_std_23)

    # Link libraries: nanobind, Python, GLFW, OpenGL
    target_link_libraries(_r2d2 PRIVATE ${NANOBIND_TARGET} Python::Python ${GLFW_LIBRARIES} OpenGL::GL)

    # Ensure Python can import the extension as r2d2._r2d2
    set_target_properties(_r2d2 PROPERTIES PREFIX "")

    # Install rule (optional) - place in Python package layout if using install
    install(TARGETS _r2d2
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(BUILD_DEMO)
    add_executable(r2d2_demo
        ${CMAKE_SOURCE_DIR}/examples/demo/main.cpp
        ${ENGINE_CPP_SRCS}
        ${ENGINE_C_SRCS}
    )
    target_include_directories(r2d2_demo PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/engine/core
        ${GLFW_INCLUDEDIR}
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
        ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/include
    )
    target_compile_features(r2d2_demo PRIVATE cxx_std_23)
    target_link_libraries(r2d2_demo PRIVATE ${GLFW_LIBRARIES} OpenGL::GL)
endif()
