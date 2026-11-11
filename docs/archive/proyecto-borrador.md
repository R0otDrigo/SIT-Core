# Simulador de Gestión y Optimización de Flotas de Transporte Público

## Contexto: SIT Arequipa

## 1. Objetivo del proyecto

Desarrollar una aplicación de escritorio en **C++ moderno + Qt 6** que simule un sistema de gestión y optimización de una flota de transporte público inspirado en el contexto del **Sistema Integrado de Transportes (SIT) de Arequipa**.

La aplicación será un **simulador**, no un sistema oficial ni una representación exacta del SIT real.

El sistema debe simular:

* Buses.
* Rutas.
* Paraderos.
* Pasajeros y demanda.
* Tráfico.
* Incidentes.
* Movimiento de vehículos.
* Despacho de buses.
* Cambios dinámicos de velocidad.
* Recalculo/cambio de rutas.
* Estadísticas de operación.
* Escenarios de simulación.

El objetivo académico es demostrar:

* Programación Orientada a Objetos.
* C++ moderno.
* Arquitectura MVC.
* Patrones de diseño.
* Herencia y polimorfismo.
* Smart pointers.
* Templates.
* Lambdas.
* Sobrecarga de operadores.
* Manejo de errores.
* Multithreading.
* Desarrollo de GUI con Qt 6.

---

# 2. Tecnologías

## Lenguaje

* C++20 como estándar preferido.

## Framework

* Qt Quick/qml + c++

## Build system

* CMake.

## Módulos Qt principales


---

# 3. Restricciones importantes

## 3.1 No implementar un sistema real del SIT

El proyecto debe ser claramente un simulador.

No depender de:

* APIs oficiales del SIT.
* GPS real.
* Datos privados.
* Servicios externos obligatorios.

---

# 4. Arquitectura

Utilizar estrictamente una arquitectura **MVC**.

```text
src/
├── model/
├── view/
├── controller/
├── patterns/
├── simulation/
├── utils/
└── main.cpp
```

## Model

Contiene el estado y las entidades del sistema.

No debe contener código específico de la interfaz gráfica.

Ejemplos:

```text
Vehicle
Bus
FeederBus
TrunkBus
Route
Stop
Driver
Passenger
TrafficCondition
Incident
Fleet
SimulationState
```

## View

Contiene exclusivamente la interfaz Qt.

Ejemplos:

```text
MainWindow
MapView
DashboardWidget
FleetPanel
TrafficPanel
SimulationControls
```

La View no debe implementar lógica de negocio.

## Controller

Coordina Model y View.

Ejemplos:

```text
SimulationController
DispatchController
TrafficController
RouteController
```

Los Controllers reciben acciones de la View, modifican el Model y notifican cambios a la View.

---

# 5. Estructura propuesta

```text
src/
├── main.cpp
│
├── model/
│   ├── Vehicle.h
│   ├── Vehicle.cpp
│   ├── Bus.h
│   ├── Bus.cpp
│   ├── TrunkBus.h
│   ├── TrunkBus.cpp
│   ├── FeederBus.h
│   ├── FeederBus.cpp
│   ├── Route.h
│   ├── Route.cpp
│   ├── Stop.h
│   ├── Stop.cpp
│   ├── Driver.h
│   ├── Driver.cpp
│   ├── Passenger.h
│   ├── Passenger.cpp
│   ├── Fleet.h
│   ├── Fleet.cpp
│   ├── Traffic.h
│   ├── Traffic.cpp
│   ├── Incident.h
│   └── Incident.cpp
│
├── controller/
│   ├── SimulationController.h
│   ├── SimulationController.cpp
│   ├── DispatchController.h
│   ├── DispatchController.cpp
│   ├── TrafficController.h
│   ├── TrafficController.cpp
│   ├── RouteController.h
│   └── RouteController.cpp
│
├── simulation/
│   ├── SimulationEngine.h
│   ├── SimulationEngine.cpp
│   ├── SimulationWorker.h
│   └── SimulationWorker.cpp
│
├── patterns/
│   ├── VehicleFactory.h
│   ├── VehicleFactory.cpp
│   ├── RouteDecorator.h
│   ├── RouteDecorator.cpp
│   ├── TrafficDecorator.h
│   ├── TrafficDecorator.cpp
│   ├── AccidentDecorator.h
│   └── AccidentDecorator.cpp
│
├── view/
│   ├── MainWindow.h
│   ├── MainWindow.cpp
│   ├── MainWindow.ui
│   ├── MapView.h
│   ├── MapView.cpp
│   ├── DashboardWidget.h
│   ├── DashboardWidget.cpp
│   ├── FleetPanel.h
│   ├── FleetPanel.cpp
│   ├── TrafficPanel.h
│   ├── TrafficPanel.cpp
│   ├── SimulationControls.h
│   └── SimulationControls.cpp
│
└── utils/
    ├── Logger.h
    ├── Constants.h
    └── Types.h
```

La estructura puede modificarse si existe una razón arquitectónica clara.

No crear archivos innecesarios únicamente para aumentar la cantidad de clases.

---

# 6. Modelo de vehículos

Crear una clase abstracta `Vehicle`.

Conceptualmente:

```cpp
class Vehicle {
public:
    virtual ~Vehicle() = default;

    virtual double getSpeed() const = 0;
    virtual int getCapacity() const = 0;
    virtual void update(double deltaTime) = 0;
};
```

Debe existir herencia y polimorfismo.

Jerarquía inicial:

```text
Vehicle
   │
   └── Bus
        │
        ├── TrunkBus
        └── FeederBus
```

Se pueden agregar otros vehículos posteriormente si aportan al proyecto.

---

# 7. Smart pointers

Utilizar C++ moderno.

Preferir:

```cpp
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

según el caso.

Evitar:

```cpp
new
delete
```

manuales.

La flota puede utilizar:

```cpp
std::vector<std::unique_ptr<Vehicle>>
```

cuando la propiedad de los vehículos corresponda exclusivamente a la flota.

---

# 8. Rutas

Crear una clase `Route`.

Una ruta contiene una secuencia ordenada de paraderos.

Ejemplo conceptual:

```text
Ruta 1

Stop A
  ↓
Stop B
  ↓
Stop C
  ↓
Stop D
```

La clase debe permitir:

* agregar paraderos.
* consultar paraderos.
* obtener un paradero por índice.
* conocer la longitud de la ruta.
* determinar la posición de un vehículo.
* avanzar al siguiente paradero.

---

# 9. Paraderos

Crear clase `Stop`.

Debe contener como mínimo:

```text
id
name
position
waitingPassengers
```

La posición puede representarse mediante:

```cpp
QPointF
```

si resulta conveniente para la integración con `QGraphicsScene`.

No acoplar innecesariamente el Model a Widgets.

---

# 10. Demanda de pasajeros

Cada paradero debe mantener una cantidad de pasajeros esperando.

Ejemplo:

```text
Stop A → 12
Stop B → 57
Stop C → 4
```

La demanda debe cambiar dinámicamente durante la simulación.

Puede utilizarse generación pseudoaleatoria controlada.

Ejemplo:

```text
Hora punta:
alta generación de pasajeros.

Hora normal:
generación media.

Hora nocturna:
generación baja.
```

No es necesario simular pasajeros individuales inicialmente.

Utilizar cantidades agregadas para mantener el sistema simple y eficiente.

---

# 11. Sistema de despacho

Crear un `DispatchController`.

Su responsabilidad será analizar el estado de los paraderos y decidir cuándo despachar vehículos.

Primera versión:

```text
si demanda > threshold
    solicitar bus
```

Posteriormente implementar una puntuación de prioridad.

Ejemplo conceptual:

```text
priority =
    demandScore
    + waitingTimeScore
    + distanceScore
    + trafficScore
```

La fórmula exacta puede modificarse durante el desarrollo.

El sistema debe poder identificar el paradero con mayor prioridad y seleccionar un vehículo disponible.

---

# 12. Factory Method

Implementar un patrón creacional para crear vehículos.

Crear:

```text
VehicleFactory
```

Debe permitir solicitar vehículos sin que el código cliente tenga que conocer directamente las clases concretas.

Ejemplo conceptual:

```cpp
auto vehicle = VehicleFactory::create(VehicleType::TrunkBus);
```

Debe devolver preferiblemente:

```cpp
std::unique_ptr<Vehicle>
```

Tipos iniciales:

```text
TrunkBus
FeederBus
```

No agregar tipos adicionales si no aportan funcionalidad.

---

# 13. Decorator

Implementar un patrón estructural para modificar dinámicamente las condiciones de una ruta.

Conceptualmente:

```text
Route
  ↓
TrafficDecorator
  ↓
AccidentDecorator
```

Ejemplo:

```text
Ruta normal
40 km/h

+
Tráfico pesado

20 km/h

+
Accidente

5 km/h
```

El objetivo es poder agregar condiciones sin modificar directamente la clase base de ruta.

Posibles decoradores:

```text
TrafficDecorator
AccidentDecorator
RoadClosureDecorator
```

No es obligatorio implementar todos.

Como mínimo:

```text
TrafficDecorator
AccidentDecorator
```

---


La interfaz Qt debe permanecer responsiva mientras la simulación está ejecutándose.



# 17. Lambdas

Utilizar lambdas de forma significativa.

Ejemplo:

```cpp
QTimer::singleShot(5000, [&]() {
    // activar incidente
});
```

Posibles usos:

* eventos temporales.
* callbacks.
* conexiones Qt.
* acciones de botones.
* generación de eventos de simulación.

No utilizar lambdas únicamente para cumplir el requisito; deben tener una función real.

---

# 18. Sobrecarga de operadores

Implementar al menos una sobrecarga significativa.

Se puede utilizar:

```cpp
++bus;
```

para representar el ingreso de un pasajero.

Ejemplo conceptual:

```cpp
Bus& Bus::operator++();
```

Debe respetar la capacidad máxima del vehículo.

Sin embargo, la API principal también debe tener un método explícito:

```cpp
bus.boardPassenger();
```

La sobrecarga se utilizará principalmente como demostración de C++.

---

# 19. Manejo de errores

Implementar manejo de errores para situaciones como:

```text
Bus lleno
Ruta inexistente
Paradero inexistente
Índice inválido
Vehículo no disponible
Ruta bloqueada
Datos de simulación inválidos
```


No utilizar excepciones para controlar flujo normal.

---

---

# 21. Vista gráfica

Poder implementar inicialmente sobre el mapa de Arequipa - Arequipa
---

# 22. Buses en el mapa

Cada vehículo debe tener una representación visual.


# 23. Dashboard

Crear un panel con estadísticas:

```text
Buses activos
Buses detenidos
Pasajeros esperando
Pasajeros transportados
Incidentes activos
Paraderos congestionados
Demanda actual
Tiempo promedio de espera
```

Actualizarlo mediante señales.

---


# 25. Controles de simulación

La interfaz debe tener:

```text
Iniciar
Pausar
Reanudar
Reiniciar
```

Además:

```text
Velocidad:

0.5x
1x
2x
5x
10x
```

La velocidad debe afectar únicamente al tiempo de simulación.

---

# 26. Escenarios

Implementar inicialmente algunos escenarios predefinidos:

```tex
```
