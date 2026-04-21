cmake_minimum_required(VERSION 3.20)
project(prueba2)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib)

# QUITAMOS Login.h de aquí
add_executable(prueba2 main.cpp)

target_link_libraries(prueba2 raylib opengl32 gdi32 winmm)