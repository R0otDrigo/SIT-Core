# Libreria del modelo: C++ puro, SIN Qt (asi se puede probar facil).
add_library(sit_model STATIC
  model/Vehicle.cpp
  model/VehicleFactory.cpp
  model/Stop.cpp
)
target_include_directories(sit_model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Aplicacion de escritorio: solo se construye si Qt6 esta instalado.
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  set(CMAKE_AUTOMOC ON)
  add_executable(sit_core main.cpp)
  target_link_libraries(sit_core PRIVATE sit_model Qt6::Widgets)
else()
  message(STATUS "Qt6 no encontrado: se omite la app, solo modelo y tests.")
endif()
