cmake_minimum_required(VERSION 3.20)

project(livecompile)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(
    app
    src/main.cpp
)

add_library(
    game
    SHARED
    src/game.cpp
)

target_compile_features(app PRIVATE cxx_std_20)
target_compile_features(game PRIVATE cxx_std_20)

set_target_properties(
    app
    game
    PROPERTIES
    CXX_EXTENSIONS OFF
)
