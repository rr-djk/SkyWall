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

add_library(ipc_lib
    src/ipc/Serializer.cpp
    src/ipc/PipeComm.cpp
)

target_include_directories(ipc_lib PUBLIC src/)

add_library(engine_lib
    src/engine/GameEngine.cpp
    src/engine/Board.cpp
    src/engine/Renderer.cpp
    src/engine/PlayerProcess.cpp
)

target_include_directories(engine_lib PUBLIC src/)
target_link_libraries(engine_lib ipc_lib ${CURSES_LIBRARIES})

# ---------------------------------------------------------------------------
# Exécutable principal
# ---------------------------------------------------------------------------

add_executable(skywall src/engine/main.cpp)
target_link_libraries(skywall engine_lib)

# ---------------------------------------------------------------------------
# Exécutable joueur
# ---------------------------------------------------------------------------

add_executable(skywall_player
    src/player/PlayerRunner.cpp
    src/player/player.cpp
)
target_include_directories(skywall_player PRIVATE src/)
target_link_libraries(skywall_player ipc_lib)

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

add_executable(test_Serializer tests/test_Serializer.cpp)
target_link_libraries(test_Serializer ipc_lib gtest_main)
add_test(NAME test_Serializer COMMAND test_Serializer)

add_executable(test_PipeComm tests/test_PipeComm.cpp)
target_link_libraries(test_PipeComm ipc_lib gtest_main)
add_test(NAME test_PipeComm COMMAND test_PipeComm)

add_executable(test_PlayerProcess tests/test_PlayerProcess.cpp)
target_link_libraries(test_PlayerProcess engine_lib ipc_lib gtest_main)
add_test(NAME test_PlayerProcess COMMAND test_PlayerProcess)

add_executable(test_player tests/test_player.cpp src/player/player.cpp)
target_include_directories(test_player PRIVATE src/)
target_link_libraries(test_player ipc_lib gtest_main)
add_test(NAME test_player COMMAND test_player)

add_custom_target(unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_Board test_Serializer test_PipeComm test_PlayerProcess test_player
)
