cmake_minimum_required(VERSION 3.20)
project(prueba2)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib)

# Buscar CURL (usa mingw64)
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
find_package(CURL REQUIRED)

add_executable(prueba2
        main.cpp
        api.cpp
        requests.cpp
        Puntaje.h
        Puntaje.h
        Puntaje.h
)

target_link_libraries(prueba2
        raylib
        CURL::libcurl
        opengl32
        gdi32
        winmm
        ws2_32
        wldap32
        crypt32
        normaliz
)