cmake_minimum_required(VERSION 3.21)

project(teo_to
    VERSION 0.1.0
    DESCRIPTION "Simulador de gestion y optimizacion de flotas de transporte publico (SIT Arequipa)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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

qt_add_executable(teo_to
    src/main.cpp
    src/model/Stop.cpp
    src/model/Route.cpp
    src/model/Bus.cpp
    src/model/TrunkBus.cpp
    src/model/FeederBus.cpp
    src/model/Fleet.cpp
    src/viewmodel/SimulationView.cpp
    src/utils/Logger.cpp
    resources.qrc
)

target_include_directories(teo_to PRIVATE src)

target_link_libraries(teo_to PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Location
    Qt6::Positioning
    Qt6::Network
)

set_target_properties(teo_to PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
