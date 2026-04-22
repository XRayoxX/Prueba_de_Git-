cmake_minimum_required(VERSION 3.20)
project(prueba2)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib)

set(OPENSSL_ROOT_DIR "C:/msys64/ucrt64")
set(OPENSSL_INCLUDE_DIR "C:/msys64/ucrt64/include")
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

find_package(CURL REQUIRED)

add_executable(prueba2
        main.cpp
        api.cpp
        requests.cpp
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