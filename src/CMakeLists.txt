add_library(teoto_model STATIC
    model/Stop.cpp
    model/Route.cpp
    model/Bus.cpp
    model/TrunkBus.cpp
    model/FeederBus.cpp
    model/Fleet.cpp
    model/VehicleFactory.cpp
)

target_include_directories(teoto_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(teoto_model PUBLIC Qt6::Core Qt6::Positioning)

add_executable(teo_to
    main.cpp
    viewmodel/SimulationView.cpp
    utils/Logger.cpp
    ${PROJECT_SOURCE_DIR}/resources.qrc
)

target_link_libraries(teo_to PRIVATE
    teoto_model
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
