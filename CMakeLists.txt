cmake_minimum_required(VERSION 3.20)
project(R2D2Engine2D LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
include(GNUInstallDirs)
include(CTest)

# Options de compilation
option(BUILD_PY_BINDINGS "Build Python bindings (nanobind)" ON)
option(BUILD_DEMO "Build demo executable" ON)

# Dépendances système via PkgConfig et CMake standard
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
find_package(OpenGL REQUIRED)

# ==============================================================================
# 1. ARCHITECTURE CENTRALISÉE : LA BIBLIOTHÈQUE DU MOTEUR (r2d2_core)
# ==============================================================================
file(GLOB_RECURSE ENGINE_CPP_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/engine/core/*.cpp")
file(GLOB_RECURSE ENGINE_C_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/engine/core/*.c")
list(REMOVE_DUPLICATES ENGINE_C_SRCS)

add_library(r2d2_core STATIC
    ${ENGINE_CPP_SRCS}
    ${ENGINE_C_SRCS}
)

target_include_directories(r2d2_core PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/core
    ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3
    ${CMAKE_SOURCE_DIR}/engine/core/backend/gl3/glad
)

target_compile_features(r2d2_core PUBLIC cxx_std_23)
target_link_libraries(r2d2_core PUBLIC PkgConfig::GLFW OpenGL::GL)

# ==============================================================================
# 2. MODULE PYTHON (Verrouillé de force sur la version Free-Threaded t sans GIL)
# ==============================================================================
if(BUILD_PY_BINDINGS)
    set(Python_ROOT_DIR "/opt/python3148t")
    set(Python_FIND_STRATEGY LOCATION)

    find_package(Python 3 REQUIRED COMPONENTS Interpreter Development.Module)
    message(STATUS "🔥 RECONNEXION MULTI-THREAD : Python Free-Threaded détecté : ${Python_VERSION}")

    # Récupération automatique avec isolation des sous-cibles de nanobind
    include(FetchContent)
    FetchContent_Declare(
        nanobind
        GIT_REPOSITORY https://github.com/wjakob/nanobind.git
        GIT_TAG        v2.7.0        # Remise de la version exacte d'Ethan
        GIT_SHALLOW    YES
    )

    # FIX INDISPENSABLE : Empêche nanobind de polluer le build global et de crasher
    set(NANOBIND_BUILD_TESTS OFF CACHE INTERNAL "")
    set(NANOBIND_BUILD_EXAMPLES OFF CACHE INTERNAL "")

    FetchContent_MakeAvailable(nanobind)

    # Compilation des bindings
    nanobind_add_module(_r2d2 NB_SHARED
        bindings/bindings.cpp
    )

    target_include_directories(_r2d2 PRIVATE
        ${Python_INCLUDE_DIRS}
        ${nanobind_SOURCE_DIR}/include
    )

    target_link_libraries(_r2d2 PRIVATE r2d2_core)

    set_target_properties(_r2d2 PROPERTIES
        PREFIX ""
        INTERPROCEDURAL_OPTIMIZATION OFF
    )

    install(TARGETS _r2d2 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

# ==============================================================================
# 3. EXÉCUTABLE DE DÉMO NATIVE
# ==============================================================================
if(BUILD_DEMO)
    set(DEMO_MAIN "${CMAKE_SOURCE_DIR}/examples/demo/main.cpp")
    if(EXISTS "${DEMO_MAIN}")
        add_executable(r2d2_demo "${DEMO_MAIN}")
        target_link_libraries(r2d2_demo PRIVATE r2d2_core)
    else()
        message(WARNING "BUILD_DEMO enabled but examples/demo/main.cpp not found — skipping demo target")
    endif()
endif()

# ==============================================================================
# 4. EXÉCUTABLE DE TEST DE PERFORMANCE (Liaison corrigée)
# ==============================================================================
add_executable(r2d2_performance_test
    tests/performance_profile_test.cpp
)

target_link_libraries(r2d2_performance_test PRIVATE r2d2_core)

add_test(NAME r2d2_performance_test COMMAND r2d2_performance_test)
