cmake_minimum_required(VERSION 3.16)
project(ejercicios_hilos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Biblioteca de hilos del sistema (en Linux suele implicar -pthread).
find_package(Threads REQUIRED)

foreach(ejercicio
        ejercicio1_contador
        ejercicio2_sumador
        ejercicio3_lambda
        ejercicio4_sincronizacion)
    add_executable(${ejercicio} ${ejercicio}.cpp)
    target_link_libraries(${ejercicio} PRIVATE Threads::Threads)
    target_compile_options(${ejercicio} PRIVATE -Wall -Wextra -Wpedantic)
endforeach()
