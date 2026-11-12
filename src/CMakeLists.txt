# ---------------------------------------------------------------------------
# Modelo: C++ puro, SIN Qt (se puede probar sin interfaz grafica).
# Se compilan todos los .cpp de model/: al agregar un archivo nuevo no hay que editar este CMake.
# ---------------------------------------------------------------------------
file(GLOB SIT_MODEL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/model/*.cpp)
add_library(sit_model STATIC ${SIT_MODEL_SOURCES})
target_include_directories(sit_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sit_model PUBLIC Threads::Threads)

# ---------------------------------------------------------------------------
# Controlador + vistas + aplicacion: solo si Qt6 esta instalado y existen los archivos.
# ---------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets QUIET)
file(GLOB SIT_UI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/controller/*.cpp
                                            ${CMAKE_CURRENT_SOURCE_DIR}/view/*.cpp)
if(Qt6_FOUND AND SIT_UI_SOURCES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
  set(CMAKE_AUTOMOC ON)
  add_library(sit_ui STATIC ${SIT_UI_SOURCES})
  target_link_libraries(sit_ui PUBLIC sit_model Qt6::Widgets)

  add_executable(sit_core main.cpp)
  target_link_libraries(sit_core PRIVATE sit_ui)
  target_compile_definitions(sit_core PRIVATE SIT_DATA_DIR="${PROJECT_SOURCE_DIR}/data")
  add_custom_command(TARGET sit_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data $<TARGET_FILE_DIR:sit_core>/data)
else()
  message(STATUS "Aplicacion grafica omitida (falta Qt6 o los archivos de controller/view/main.cpp).")
endif()
