cmake_minimum_required(VERSION 3.16)
project(SIT-Core VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Pantalla de carga: por defecto un QDialog animado en C++ (robusto en cualquier equipo).
# Con -DSIT_QML_LOADING=ON se usa cargando.qml mediante QQuickWidget.
option(SIT_QML_LOADING "Usar cargando.qml (QQuickWidget) en la pantalla de carga" OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
if(SIT_QML_LOADING)
    find_package(Qt6 REQUIRED COMPONENTS Quick QuickWidgets)
endif()

set(SIT_SOURCES
    main.cpp
    roles.h
    theme.h
    theme.cpp
    uihelpers.h
    uihelpers.cpp
    loadingdialog.h
    loadingdialog.cpp
    firebaseauthmanager.h
    firebaseauthmanager.cpp
    logindialog.h
    logindialog.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    dashboardbase.h
    dashboardbase.cpp
    admindashboard.h
    admindashboard.cpp
    userdashboard.h
    userdashboard.cpp
    trafficdashboard.h
    trafficdashboard.cpp
    movementdashboard.h
    movementdashboard.cpp
    arequipamapwidget.h
    arequipamapwidget.cpp
    guestdashboard.h
    guestdashboard.cpp
    resources.qrc
)

add_executable(SIT-Core ${SIT_SOURCES})

target_link_libraries(SIT-Core PRIVATE Qt6::Widgets Qt6::Network)

if(SIT_QML_LOADING)
    target_compile_definitions(SIT-Core PRIVATE SIT_QML_LOADING)
    target_link_libraries(SIT-Core PRIVATE Qt6::Quick Qt6::QuickWidgets)
endif()

set_target_properties(SIT-Core PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
