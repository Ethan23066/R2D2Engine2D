cmake_minimum_required(VERSION 3.21)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

include(GNUInstallDirs)
include(CTest)
include(FetchContent)

option(BUILD_PY_BINDINGS "Build Python bindings (nanobind, Python 3.14t)" ON)
option(BUILD_DEMO "Build demo executable when its source is available" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# Core native library
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/core/*.cpp")
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/core/*.c")
list(REMOVE_DUPLICATES ENGINE_C_SRCS)

add_library(r2d2_core STATIC
    ${ENGINE_CPP_SRCS}
    ${ENGINE_C_SRCS}
)

target_include_directories(r2d2_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core/backend/gl3
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core/backend/gl3/glad
)

target_compile_features(r2d2_core PUBLIC cxx_std_23)
target_link_libraries(r2d2_core PUBLIC
    PkgConfig::GLFW
    OpenGL::GL
    Threads::Threads
)

# Python Free-Threaded bindings.
# FindPython gained explicit support for the fourth (gil_disabled) ABI flag in CMake 3.30.
if(BUILD_PY_BINDINGS)
    if(CMAKE_VERSION VERSION_LESS 3.30)
        message(FATAL_ERROR
            "BUILD_PY_BINDINGS requires CMake 3.30 or newer to select Python's "
            "Free-Threaded ABI reliably. Use -DBUILD_PY_BINDINGS=OFF for a native-only build.")
    endif()

    set(Python_FIND_ABI "OFF" "ANY" "ANY" "ON")
    set(Python_FIND_STRATEGY LOCATION)
    find_package(Python 3.14 REQUIRED COMPONENTS Interpreter Development.Module)

    if(NOT Python_VERSION VERSION_GREATER_EQUAL 3.14)
        message(FATAL_ERROR "Python 3.14 or newer is required for the bindings.")
    endif()

    message(STATUS "Python interpreter: ${Python_EXECUTABLE}")
    message(STATUS "Python version: ${Python_VERSION}")
    message(STATUS "Python Free-Threaded ABI requested.")

    # Pin the dependency: never build against a moving master branch.
    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind.git
        GIT_TAG        v3.1.0
        GIT_SHALLOW    TRUE
        GIT_SUBMODULES ext/robin_map
        GIT_SUBMODULES_RECURSE TRUE
    )

    set(NB_TEST OFF CACHE BOOL "Disable nanobind's own test suite" FORCE)
    set(NB_CREATE_INSTALL_RULES OFF CACHE BOOL "Do not install nanobind separately" FORCE)
    set(NB_USE_SUBMODULE_DEPS ON CACHE BOOL "Use nanobind's pinned robin-map submodule" FORCE)

    FetchContent_MakeAvailable(nanobind)

    nanobind_add_module(_r2d2
        NB_SHARED
        FREE_THREADED
        bindings/bindings.cpp
    )

    target_link_libraries(_r2d2 PRIVATE r2d2_core)
    set_target_properties(_r2d2 PROPERTIES
        PREFIX ""
        INTERPROCEDURAL_OPTIMIZATION OFF
    )

    install(TARGETS _r2d2
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Native demo
if(BUILD_DEMO)
    set(DEMO_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/examples/demo/main.cpp")
    if(EXISTS "${DEMO_MAIN}")
        add_executable(r2d2_demo "${DEMO_MAIN}")
        target_link_libraries(r2d2_demo PRIVATE r2d2_core)
    else()
        message(STATUS "BUILD_DEMO enabled but examples/demo/main.cpp is absent; skipping r2d2_demo.")
    endif()
endif()

# Tests
if(BUILD_TESTING)
    add_executable(r2d2_performance_test tests/performance_profile_test.cpp)
    target_link_libraries(r2d2_performance_test PRIVATE r2d2_core)
    add_test(NAME r2d2_performance_test COMMAND r2d2_performance_test)
endif()
