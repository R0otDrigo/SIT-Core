cmake_minimum_required(VERSION 3.16)

project(TecnologiaObjetos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 COMPONENTS Quick Widgets LinguistTools Network REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

qt_add_executable(appTecnologiaObjetos
    main.cpp

    firebaseauthmanager.h
    firebaseauthmanager.cpp
    admindashboard.h
    admindashboard.cpp
    userdashboard.h
    userdashboard.cpp
    guestdashboard.h
    guestdashboard.cpp
    login.h
    login.cpp
    login.ui
    signup.h
    signup.ui
    signup.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
)

qt_add_translations(appTecnologiaObjetos
    TS_FILES
        TecnologiaObjetos_es_ES.ts
        TecnologiaObjetos_en_US.ts
        TecnologiaObjetos_zh_CN.ts
        TecnologiaObjetos_ar_SA.ts
)

qt_add_qml_module(appTecnologiaObjetos
    URI TecnologiaObjetos
    VERSION 1.0
    QML_FILES
      main.qml
      cargando.qml
)

# Adding a config.ini to firebaseauthmanager


add_custom_command(TARGET appTecnologiaObjetos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:appTecnologiaObjetos>/config.ini")


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

#set(FIREBASE_CPP_SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/3rdparty/firebase-cpp-sdk-14.1.0")
#add_subdirectory(${FIREBASE_CPP_SDK_DIR} bin/ EXCLUDE_FROM_ALL)

#target_link_libraries(${target_name} PRIVATE
#    firebase_auth
#    firebase_database
#    firebase_app
#)

include(GNUInstallDirs)
install(TARGETS appTecnologiaObjetos
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
