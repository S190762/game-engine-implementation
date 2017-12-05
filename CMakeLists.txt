cmake_minimum_required(VERSION 3.7)
project(GameEngine)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DSFML_STATIC -Wno-narrowing)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILES  main.cpp)
add_executable(GameEngine ${SOURCE_FILES})
target_link_libraries(GameEngine sfgui sfml-graphics sfml-window sfml-system)