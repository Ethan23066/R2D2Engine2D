cmake_minimum_required(VERSION 3.21)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CTest)
include(FetchContent)

option(BUILD_PY_BINDINGS "Build Python bindings (Free-Threaded preferred, GIL fallback)" ON)
option(BUILD_DEMO "Build demo executable when its source is available" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(stb)

file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/engine/core/*.cpp")
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/engine/core/*.c")
list(REMOVE_DUPLICATES ENGINE_C_SRCS)

add_library(r2d2_core STATIC ${ENGINE_CPP_SRCS} ${ENGINE_C_SRCS})

target_include_directories(r2d2_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core/backend/gl3
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core/backend/gl3/glad
    ${stb_SOURCE_DIR}
)

target_compile_features(r2d2_core PUBLIC cxx_std_23)
target_link_libraries(r2d2_core PUBLIC PkgConfig::GLFW OpenGL::GL Threads::Threads)

if(BUILD_PY_BINDINGS)
    set(Python_FIND_UNVERSIONED_NAMES FIRST)
    find_package(Python 3.12 REQUIRED COMPONENTS Interpreter Development.Module)

    execute_process(
        COMMAND "${Python_EXECUTABLE}" -c "import sysconfig; print(sysconfig.get_config_var('Py_GIL_DISABLED') or 0)"
        OUTPUT_VARIABLE R2D2_PY_GIL_DISABLED
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(R2D2_PY_GIL_DISABLED STREQUAL "1")
        set(R2D2_PYTHON_FREE_THREADED ON)
    else()
        set(R2D2_PYTHON_FREE_THREADED OFF)
    endif()

    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind
        GIT_TAG        v3.1.0
        GIT_SHALLOW    TRUE
    )
    set(NB_TEST OFF CACHE BOOL "" FORCE)
    set(NB_CREATE_INSTALL_RULES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(nanobind)

    if(R2D2_PYTHON_FREE_THREADED)
        nanobind_add_module(_r2d2 NB_SHARED FREE_THREADED bindings/bindings.cpp)
    else()
        nanobind_add_module(_r2d2 NB_SHARED bindings/bindings.cpp)
    endif()

    target_link_libraries(_r2d2 PRIVATE r2d2_core)
    set_target_properties(_r2d2 PROPERTIES PREFIX "")
    install(TARGETS _r2d2 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(BUILD_DEMO)
    set(DEMO_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/examples/demo/main.cpp")
    if(EXISTS "${DEMO_MAIN}")
        add_executable(r2d2_demo "${DEMO_MAIN}")
        target_link_libraries(r2d2_demo PRIVATE r2d2_core)
    endif()
endif()

if(BUILD_TESTING)
    add_executable(r2d2_performance_test tests/performance_profile_test.cpp)
    target_link_libraries(r2d2_performance_test PRIVATE r2d2_core)
    add_test(NAME r2d2_performance_test COMMAND r2d2_performance_test)
endif()
