cmake_minimum_required(VERSION 3.21)

project(TEO_TO
    VERSION 0.2.0
    DESCRIPTION "Simulador de transporte publico inspirado en el SIT de Arequipa"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Location
    Positioning
    Network
)

qt_standard_project_setup(REQUIRES 6.5)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
