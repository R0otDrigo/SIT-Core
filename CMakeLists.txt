cmake_minimum_required(VERSION 3.16)

project(TecnologiaObjetos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 COMPONENTS Quick Widgets Network REQUIRED)

qt_add_executable(appTecnologiaObjetos
    main.cpp
    mainwindow.ui
    logindialog.h
    logindialog.cpp
    firebaseauthmanager.h
    firebaseauthmanager.cpp
    admindashboard.h
    admindashboard.cpp
    userdashboard.h
    userdashboard.cpp
    guestdashboard.h
    guestdashboard.cpp
)

qt_add_qml_module(appTecnologiaObjetos
    URI TecnologiaObjetos
    VERSION 1.0
    QML_FILES
      main.qml
      cargando.qml
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appTecnologiaObjetos PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTecnologiaObjetos
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appTecnologiaObjetos
    PRIVATE Qt6::Quick Qt6::Widgets Qt6::Network)

include(GNUInstallDirs)
install(TARGETS appTecnologiaObjetos
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
