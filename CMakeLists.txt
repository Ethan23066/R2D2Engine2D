cmake_minimum_required(VERSION 3.20)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
include(GNUInstallDirs)
include(CTest)

# Build options
option(BUILD_PY_BINDINGS "Build Python bindings (nanobind)" ON)
option(BUILD_DEMO "Build demo executable" ON)

# External deps
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

# Collect engine sources.
# This includes all engine/core files, including new render/thread/performance layers.
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.cpp"
)
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/engine/core/*.c"
)
list(APPEND ENGINE_C_SRCS "${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad/glad.c")

# Python & nanobind
if(BUILD_PY_BINDINGS)
    execute_process(
        COMMAND bash -lc "ls -1 /opt/python*/bin/python3* 2>/dev/null | grep -E 'python3\\.[0-9]+t$' | sort -V | tail -n 1"
        RESULT_VARIABLE _python_t_status
        OUTPUT_VARIABLE _python_t_exec
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT _python_t_status EQUAL 0 OR "${_python_t_exec}" STREQUAL "")
        execute_process(
            COMMAND bash -lc "ls -1 /opt/python*/bin/python3* 2>/dev/null | sort -V | tail -n 1"
            RESULT_VARIABLE _python_status
            OUTPUT_VARIABLE _python_exec
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else()
        set(_python_status 0)
        set(_python_exec "${_python_t_exec}")
    endif()

    if(NOT _python_status EQUAL 0 OR "${_python_exec}" STREQUAL "")
        message(FATAL_ERROR "No suitable Python interpreter found under /opt/python*/bin")
    endif()

    set(Python_EXECUTABLE "${_python_exec}" CACHE FILEPATH "Python interpreter" FORCE)
    message(STATUS "Using Python executable: ${Python_EXECUTABLE}")

    get_filename_component(Python_ROOT_DIR "${Python_EXECUTABLE}" DIRECTORY)
    get_filename_component(Python_ROOT_DIR "${Python_ROOT_DIR}" DIRECTORY)
    set(Python_ROOT_DIR "${Python_ROOT_DIR}" CACHE PATH "Python root directory" FORCE)

    find_package(Python 3 REQUIRED COMPONENTS Interpreter Development.Module)
    message(STATUS "Python version found: ${Python_VERSION}")

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
        ${nanobind_SOURCE_DIR}/include
    )

    target_compile_features(_r2d2 PRIVATE cxx_std_23)
    target_compile_options(_r2d2 PRIVATE -fno-lto)
    target_link_options(_r2d2 PRIVATE -fno-lto)

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

# Demo target
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

# Smoke test
add_executable(r2d2_performance_test
    tests/performance_profile_test.cpp
)

target_include_directories(r2d2_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/core
    ${GLFW_INCLUDEDIR}
)

target_compile_features(r2d2_performance_test PRIVATE cxx_std_23)

target_link_libraries(r2d2_performance_test PRIVATE
    ${GLFW_LIBRARIES}
    OpenGL::GL
)

add_test(NAME r2d2_performance_test COMMAND r2d2_performance_test)
