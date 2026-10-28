# ---------------------------------------------------------------------------
# Modelo: C++ puro, SIN Qt (se puede probar sin interfaz grafica).
# Se compilan todos los .cpp de model/: al agregar un archivo nuevo no hay que editar este CMake.
# ---------------------------------------------------------------------------
find_package(Threads REQUIRED)
file(GLOB SIT_MODEL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/model/*.cpp)
add_library(sit_model STATIC ${SIT_MODEL_SOURCES})
target_include_directories(sit_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sit_model PUBLIC Threads::Threads)

# ---------------------------------------------------------------------------
# Controlador + vistas + aplicacion: solo si Qt6 esta instalado.
# ---------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets Network Quick QuickWidgets Qml Positioning Location QUIET)
file(GLOB SIT_UI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/controller/*.cpp
                                            ${CMAKE_CURRENT_SOURCE_DIR}/view/*.cpp)
if(Qt6_FOUND AND SIT_UI_SOURCES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
  add_library(sit_ui STATIC ${SIT_UI_SOURCES} qml/qml.qrc)
  target_include_directories(sit_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(sit_ui PUBLIC sit_model Qt6::Widgets Qt6::Network Qt6::Quick
                                        Qt6::QuickWidgets Qt6::Qml Qt6::Positioning Qt6::Location)
  target_compile_definitions(sit_ui PRIVATE SIT_DATA_DIR="${PROJECT_SOURCE_DIR}/data")

  add_executable(sit_core main.cpp)
  target_link_libraries(sit_core PRIVATE sit_ui)
  target_compile_definitions(sit_core PRIVATE SIT_DATA_DIR="${PROJECT_SOURCE_DIR}/data")
# Traducciones (opcional, requiere Qt LinguistTools): se incrustan como recurso :/i18n/...
find_package(Qt6 QUIET COMPONENTS LinguistTools)
if(Qt6LinguistTools_FOUND)
  file(GLOB SIT_TS_FILES ${PROJECT_SOURCE_DIR}/i18n/*.ts)
  qt_add_translations(sit_core TS_FILES ${SIT_TS_FILES})
endif()

# Red de ejemplo (y config.ini de Firebase si existe) junto al ejecutable.
add_custom_command(TARGET sit_core POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data $<TARGET_FILE_DIR:sit_core>/data)
if(EXISTS ${PROJECT_SOURCE_DIR}/config.ini)
  add_custom_command(TARGET sit_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/config.ini $<TARGET_FILE_DIR:sit_core>/config.ini)
endif()
endif()
