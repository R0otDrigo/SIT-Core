cmake_minimum_required(VERSION 3.16)
project(SITCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 COMPONENTS Quick QuickWidgets Qml Widgets LinguistTools Network Positioning Location REQUIRED)

# Qt es opcional: sin Qt se compila solo el modelo y sus pruebas (ver src/CMakeLists.txt).
enable_testing()
add_subdirectory(src)
add_subdirectory(tests)