cmake_minimum_required(VERSION 3.15)
project(SkyWall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Dépendances système
# ---------------------------------------------------------------------------

find_package(Curses REQUIRED)

# ---------------------------------------------------------------------------
# Librairie interne — partagée entre l'exécutable et les tests
# ---------------------------------------------------------------------------

add_library(engine_lib
    src/engine/GameEngine.cpp
    src/engine/Board.cpp
    src/engine/Renderer.cpp
)

target_include_directories(engine_lib PUBLIC src/)
target_link_libraries(engine_lib ${CURSES_LIBRARIES})

# ---------------------------------------------------------------------------
# Exécutable principal
# ---------------------------------------------------------------------------

add_executable(skywall src/engine/main.cpp)
target_link_libraries(skywall engine_lib)

# ---------------------------------------------------------------------------
# Tests — Google Test via FetchContent (téléchargé automatiquement)
# ---------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_Board tests/test_Board.cpp)
target_link_libraries(test_Board engine_lib gtest_main)

add_test(NAME test_Board COMMAND test_Board)

add_custom_target(unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_Board
)
