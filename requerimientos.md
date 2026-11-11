# Simulador de Gestión y Optimización de Flotas de Transporte Público
## Documento de Requerimientos — inspirado en el SIT de Arequipa

| Campo | Valor |
|---|---|
| Proyecto | Simulador de flotas de transporte público (SIT Arequipa) |
| Tipo | Simulador académico (no es un sistema oficial) |
| Metodología | Prototipado evolutivo — 5 prototipos funcionales |
| Lenguaje | C++20 |
| UI | Qt 6 Quick / QML |
| Build | CMake |
| Idioma del código | Inglés |
| Idioma de la documentación | Español |
| Estado | Especificación inicial (P1 pendiente) |

---

## 1. Introducción y alcance

### 1.1 Descripción

Aplicación de escritorio que **simula** la gestión y optimización de una flota de
transporte público, inspirada en el contexto del **Sistema Integrado de Transportes
(SIT) de Arequipa**. El sistema modela buses, rutas, paraderos, pasajeros, tráfico,
incidentes, despacho de vehículos y operación en tiempo real, y presenta el estado
de la simulación sobre un mapa geográfico real.

### 1.2 Qué es

* Un **simulador** determinista y reproducible de una flota de buses.
* Una demostración de **POO, C++ moderno, arquitectura MVC/MVVM, patrones de diseño y
  concurrencia**.
* Una aplicación que se ejecuta **sin necesidad de la infraestructura real del SIT**.

### 1.3 Qué NO es

* Un sistema oficial del SIT de Arequipa.
* Una réplica exacta de rutas, tarifas o flota reales.
* Un sistema conectado a GPS, APIs oficiales o bases de datos privadas.
* Un producto comercial ni una herramienta de planificación real.

### 1.4 Alcance de esta versión

El alcance se entrega a través de **5 prototipos evolutivos** (ver §17). Este
documento describe el comportamiento objetivo **final** (prototipo P5); los
prototipos anteriores implementan subconjuntos crecientes de estos requisitos.

---

## 2. Objetivos académicos

El proyecto debe **demostrar de forma verificable** las siguientes competencias. La
tabla de trazabilidad completa (requisito → evidencia → prototipo) está en §16.

| # | Competencia | Evidencia esperada |
|---|---|---|
| OBJ-01 | Programación Orientada a Objetos | Modelo de dominio con entidades y responsabilidades claras |
| OBJ-02 | C++ moderno (C++20) | `auto`, `enum class`, `override`, `constexpr`, RAII, rangos |
| OBJ-03 | Arquitectura MVC/MVVM | Separación estricta Modelo / ViewModel / Vista |
| OBJ-04 | Patrones de diseño | Factory Method y Decorator (mínimo); Observer (señales Qt) |
| OBJ-05 | Herencia y polimorfismo | `Vehicle` → `Bus` → `TrunkBus` / `FeederBus` |
| OBJ-06 | Smart pointers | `unique_ptr`, `shared_ptr`, `weak_ptr` |
| OBJ-07 | Templates | Clases/funciones genéricas con uso real |
| OBJ-08 | Lambdas | Conexiones Qt, callbacks y eventos temporales |
| OBJ-09 | Sobrecarga de operadores | `Bus::operator++` (embarque) y otras justificadas |
| OBJ-10 | Manejo de errores | Tipos de error explícitos, sin excepciones para flujo normal |
| OBJ-11 | Multithreading | Motor de simulación en hilo de trabajo |
| OBJ-12 | GUI con Qt 6 | QML + ViewModels en C++ |

---

## 3. Tecnologías y dependencias

### 3.1 Base

| Componente | Tecnología |
|---|---|
| Lenguaje | C++20 |
| Framework | Qt 6 (Qt Quick / QML) |
| Build system | CMake ≥ 3.21 |
| Compilador probado | GCC/Clang (Linux), MSVC (Windows) |

### 3.2 Módulos Qt requeridos

| Módulo | Uso |
|---|---|
| `Qt6::Quick` | Motor QML y ventana |
| `Qt6::QuickControls2` | Controles de la interfaz |
| `Qt6::Qml` | Registro de tipos y context properties |
| `Qt6::Location` | Mapa, rutas y elementos geográficos |
| `Qt6::Positioning` | Coordenadas (`QGeoCoordinate`) |
| `Qt6::Network` | Descarga de tiles del mapa |
| `Qt6::Core` / `Qt6::Gui` | Base del modelo y utilidades |
| `Qt6::Test` | Pruebas unitarias (solo en build de tests) |

### 3.3 Herramientas opcionales

* `clang-format` para estilo de código.
* `Qt Creator` como IDE de referencia.

---

## 4. Restricciones

### 4.1 Independencia del SIT real

El proyecto **no depende** de:

* APIs oficiales del SIT.
* Datos GPS reales.
* Datos privados o bases de datos del operador.
* Servicios externos obligatorios para el funcionamiento del núcleo.

### 4.2 Mapa base (nota de alcance)

> **Cambio respecto al borrador original.** El borrador prohibía todo servicio
> externo. Para lograr un mapa realista se permite que **el mapa base** use *tiles*
> de OpenStreetMap a través de `Qt Location`. Esto es una dependencia **de
> presentación**, no del núcleo de simulación, y se mitiga con:
>
> * **Modo de respaldo (fallback):** si no hay red o el plugin `osm` no carga, el
>   mapa cae a un **modo esquemático offline** que dibuja el mismo modelo sobre un
>   canvas propio. La simulación **nunca** deja de funcionar.
> * El núcleo de simulación (Modelo, controladores, motor) **no conoce** el mapa ni
>   la red; solo publica coordenadas (`QGeoCoordinate`).
>
> El objetivo sigue siendo que el proyecto sea **demostrable sin infraestructura del
> SIT** y degradable a **100% offline** (ver RNF-01).

### 4.3 Simulador, no sistema real

La interfaz y la documentación deben dejar claro que es una simulación académica.

---

## 5. Requisitos funcionales

Prioridad: **A** = imprescindible, **M** = media, **B** = baja/opcional.
Prototipo objetivo: primera versión donde debe funcionar.

### 5.1 Flota

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-01 | El sistema debe representar una flota compuesta por vehículos de tipo `TrunkBus` y `FeederBus`. | A | P1/P2 |
| RF-02 | Se debe poder crear, consultar y eliminar vehículos de la flota. | A | P2 |
| RF-03 | Cada vehículo debe tener identificador, capacidad, velocidad máxima, estado (activo/detenido/en ruta) y ocupación actual. | A | P2 |
| RF-04 | La flota debe indicar qué vehículos están disponibles para despacho. | A | P2/P4 |

### 5.2 Rutas y paraderos

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-05 | Una `Route` debe contener una secuencia ordenada de paraderos. | A | P1 |
| RF-06 | Se debe poder agregar/consultar paraderos, obtener uno por índice, conocer la longitud de la ruta y avanzar al siguiente paradero. | A | P1/P2 |
| RF-07 | Cada `Stop` debe tener `id`, `name`, `position` (coordenadas) y `waitingPassengers`. | A | P1 |
| RF-08 | Debe existir más de una ruta y una ruta puede compartir paraderos con otra. | M | P2 |

### 5.3 Demanda de pasajeros

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-09 | Cada paradero mantiene una cantidad de pasajeros esperando (agregada, no individuos). | A | P2 |
| RF-10 | La demanda debe cambiar dinámicamente durante la simulación. | A | P4 |
| RF-11 | La generación de demanda debe modelar franjas horarias: hora punta (alta), hora normal (media), hora nocturna (baja). | A | P4 |
| RF-12 | La generación debe usar un RNG con semilla configurable para garantizar reproducibilidad. | A | P4 |

### 5.4 Movimiento y operación

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-13 | Cada vehículo se mueve a lo largo de su ruta entre paraderos, con velocidad dependiente del tráfico. | A | P1/P2 |
| RF-14 | Al llegar a un paradero, el bus debe **desembarcar** pasajeros y **embarcar** pasajeros en espera, respetando la capacidad máxima. | A | P2 |
| RF-15 | El embarque debe exponerse mediante `Bus::boardPassenger()` y mediante `Bus::operator++`. | A | P2 |
| RF-16 | Un bus lleno no debe aceptar más pasajeros; el intento debe reportarse como error, no como excepción de flujo. | A | P2 |
| RF-17 | Si un paradero no puede ser atendido por falta de capacidad, la demanda restante permanece esperando. | M | P2 |

### 5.5 Despacho

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-18 | Un `DispatchController` debe analizar el estado de los paraderos y decidir cuándo despachar vehículos. | A | P4 |
| RF-19 | La decisión inicial puede ser por umbral de demanda (`demand > threshold`). | A | P4 |
| RF-20 | Versión avanzada: puntuación de prioridad `demandScore + waitingTimeScore + distanceScore + trafficScore`. | M | P4 |
| RF-21 | El sistema debe identificar el paradero de mayor prioridad y seleccionar un vehículo disponible. | A | P4 |

### 5.6 Tráfico e incidentes

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-22 | Debe existir una entidad `TrafficCondition` que module la velocidad de una ruta. | A | P4 |
| RF-23 | Debe existir una entidad `Incident` (accidente, cierre de vía) con duración y ubicación. | A | P4 |
| RF-24 | Los incidentes deben activarse y expirar dinámicamente (temporizadores). | A | P4 |
| RF-25 | Ante una ruta bloqueada, el sistema debe recalcular/desviar el recorrido. | M | P4 |
| RF-26 | La velocidad efectiva de una ruta debe combinar las condiciones mediante decoradores. | A | P4 |

### 5.7 Simulación y control

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-27 | El sistema debe simular un reloj con franjas horarias (día) y avanzar por ticks. | A | P2/P4 |
| RF-28 | La interfaz debe ofrecer **Iniciar**, **Pausar**, **Reanudar** y **Reiniciar**. | A | P2 |
| RF-29 | La interfaz debe ofrecer multiplicadores de velocidad: 0.5x, 1x, 2x, 5x, 10x. | A | P2 |
| RF-30 | El multiplicador debe afectar **solo** al tiempo de simulación, no a la frecuencia real de refresco. | A | P3 |
| RF-31 | Debe poder registrarse un log de eventos relevantes (despachos, incidentes, bloqueos). | M | P4 |

### 5.8 Visualización

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-32 | El sistema debe mostrar un mapa geográfico de Arequipa como base. | A | P1 |
| RF-33 | Rutas y paraderos deben dibujarse sobre el mapa. | A | P3 |
| RF-34 | Cada vehículo debe tener una representación visual que se actualice en tiempo real. | A | P3 |
| RF-35 | Debe existir un mapa de respaldo esquemático si el mapa base no está disponible. | A | P1/P3 |
| RF-36 | Debe existir un panel de control (play/pausa/velocidad/reinicio). | A | P2 |
| RF-37 | Debe existir un panel de escenarios y de incidentes. | M | P4 |

### 5.9 Estadísticas (Dashboard)

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-38 | El dashboard debe mostrar: buses activos, buses detenidos, pasajeros esperando, pasajeros transportados, incidentes activos, paraderos congestionados, demanda actual y tiempo promedio de espera. | A | P3 |
| RF-39 | Las estadísticas deben actualizarse mediante señales al cambiar el estado. | A | P3 |
| RF-40 | Debe mantenerse un histórico de las métricas durante la simulación. | M | P5 |

### 5.10 Escenarios y persistencia

| ID | Requisito | Prioridad | Prototipo |
|---|---|---|---|
| RF-41 | Debe poderse cargar y guardar un **escenario** (rutas, paraderos, flota, demanda, eventos) en JSON. | A | P5 |
| RF-42 | Deben incluirse escenarios predefinidos de ejemplo. | A | P5 |
| RF-43 | Debe poderse exportar el histórico de resultados a **CSV**. | A | P5 |
| RF-44 | La carga de un escenario inválido debe reportarse con un error claro y no debe corromper el estado actual. | A | P5 |

---

## 6. Requisitos no funcionales

| ID | Requisito | Criterio verificable |
|---|---|---|
| RNF-01 | **Offline-first / degradable.** El núcleo funciona sin red; el mapa cae a modo esquemático si no hay tiles. | Con la red desconectada, la simulación arranca y avanza; se informa el modo. |
| RNF-02 | **Responsividad.** La UI nunca se congela mientras la simulación corre. | Motor en hilo de trabajo; la ventana responde a interacción durante la ejecución. |
| RNF-03 | **Determinismo.** Con la misma semilla y los mismos parámetros, el resultado es idéntico. | Dos ejecuciones con la misma semilla producen el mismo estado final; hay test. |
| RNF-04 | **Portabilidad.** Compila en Linux y Windows con C++20 y CMake. | Build limpio en ambas plataformas. |
| RNF-05 | **Rendimiento.** Soportar al menos 50 vehículos y 100 paraderos sin degradar la UI. | Frecuencia de refresco estable a 50+ vehículos. |
| RNF-06 | **Mantenibilidad.** Sin lógica de negocio en QML; el Modelo no conoce la Vista. | Revisión de capas: QML solo presentación. |
| RNF-07 | **Errores.** Sin excepciones para control de flujo (ver §11.5). | Uso de tipos de error/opciones; excepciones solo para fallos irrecuperables. |
| RNF-08 | **Usabilidad.** Controles claros, etiquetas en español, atajos básicos (espacio = pausa). | Prueba con usuario; controles evidentes. |
| RNF-09 | **Testeabilidad.** Lógica de dominio desacoplada de Qt GUI para poder probarse. | Suite de tests ejecutable con `ctest`. |
| RNF-10 | **Documentación.** README con requisitos, build, uso y guion de demo. | README completo y actualizado. |

---

## 7. Modelo de dominio

### 7.1 Entidades

| Entidad | Responsabilidad | Atributos clave |
|---|---|---|
| `Vehicle` (abstracta) | Interfaz común de vehículos | `id`, `capacity`, `maxSpeed`, `currentSpeed`, `occupancy` |
| `Bus` | Vehículo de transporte público, base intermedia | `route`, `currentStopIndex`, `state` |
| `TrunkBus` | Bus troncal (mayor capacidad) | especialización de `Bus` |
| `FeederBus` | Bus alimentador (menor capacidad) | especialización de `Bus` |
| `Route` | Secuencia ordenada de paraderos + longitud | `id`, `name`, `stops` |
| `Stop` | Paradero con demanda en espera | `id`, `name`, `position`, `waitingPassengers` |
| `Driver` | Conductor asignado a un vehículo | `id`, `name`, `shift` |
| `Passenger` | Pasajero agregado (modelo ligero) | `id`, `origin`, `destination`, `waitTime` |
| `TrafficCondition` | Condición de tráfico sobre una ruta | `type`, `speedFactor` |
| `Incident` | Evento que afecta la operación | `id`, `type`, `position`, `remainingTime` |
| `Fleet` | Contenedor y dueño de los vehículos | `vector<unique_ptr<Vehicle>>` |
| `SimulationState` | Snapshot del estado en un tick | buses, paraderos, métricas, reloj |
| `Statistics` | Métricas agregadas de operación | conteos, promedios, histórico |

### 7.2 Jerarquía de vehículos

```text
Vehicle (abstract)
   │
   └── Bus
        ├── TrunkBus
        └── FeederBus
```

### 7.3 Relaciones principales

* `Fleet` **posee** `Vehicle` (`unique_ptr`).
* `Route` **posee** `Stop` (o referencia compartida si varios recorridos comparten paraderos).
* `Bus` **referencia** su `Route` (`shared_ptr` o `weak_ptr`).
* `Incident` / `TrafficCondition` **decoran** la velocidad efectiva de una `Route`.
* `SimulationEngine` **produce** `SimulationState` y `Statistics`.

---

## 8. Arquitectura

### 8.1 Estilo: MVVM (MVC adaptado a Qt Quick)

```text
   MODELO (C++)          VIEWMODEL (C++)           VISTA (QML)
 entidades de dominio →  QObject + señales    →   presentación y animación
 sin Qt GUI              Q_PROPERTY / modelos
```

* El **Modelo** no depende de la interfaz ni de la red.
* El **ViewModel** expone el modelo a QML mediante `Q_PROPERTY`, señales y
  `QAbstractListModel`.
* La **Vista (QML)** solo presenta y emite acciones; no calcula reglas de negocio.

### 8.2 Comunicación C++ ↔ QML

| Mecanismo | Uso |
|---|---|
| `setContextProperty` | Objetos únicos (controlador principal, configuración) |
| `qmlRegisterType<T>` | Tipos instanciables desde QML |
| `qmlRegisterSingletonInstance<T>` | Acceso global tipado |
| `QAbstractListModel` | Colecciones (buses, paraderos, incidentes) para `ListView`/`Repeater`/`MapItemView` |
| `Q_PROPERTY` + `NOTIFY` | Propiedades reactivas |

### 8.3 Estructura de carpetas propuesta

```text
teo-to/
├── CMakeLists.txt
├── README.md
├── requerimientos.md
├── docs/
│   └── archive/
├── src/
│   ├── main.cpp
│   ├── model/          # entidades puras (QtCore)
│   ├── viewmodel/      # QObject expuestos a QML, list models
│   ├── controller/     # coordinación (Dispatch, Traffic, Route)
│   ├── simulation/     # SimulationEngine, SimulationWorker
│   ├── patterns/       # VehicleFactory, decoradores
│   ├── persistence/    # JSON load/save, CSV export
│   ├── utils/          # Logger, Constants, Types (errores, templates)
│   └── qml/
│       ├── Main.qml
│       ├── views/      # MapView.qml, Dashboard.qml, FleetPanel.qml...
│       └── components/ # BusItem.qml, StopItem.qml, RouteLine.qml...
├── resources/
│   ├── qml.qrc
│   └── scenarios/      # escenarios JSON predefinidos
└── tests/              # Qt Test
```

La estructura puede ajustarse por razones arquitectónicas justificadas. **No se
crean archivos innecesarios** solo para aumentar el número de clases.

---

## 9. Mapa y geolocalización

### 9.1 Mapa base

* Tipo QML `Map` con `Plugin { name: "osm" }`.
* Centro y *bounding box* aproximados de Arequipa:

| Zona | Latitud | Longitud |
|---|---|---|
| Centro (referencia) | -16.3989 | -71.5369 |
| Caja aproximada (min lat/lon) | -16.4700 | -71.6100 |
| Caja aproximada (max lat/lon) | -16.3300 | -71.4700 |

* Coordenadas de referencia (a verificar antes de usarlas):

| Lugar | Latitud | Longitud |
|---|---|---|
| Plaza de Armas | -16.3989 | -71.5369 |
| Mercado San Camilo | -16.4016 | -71.5350 |
| UNSA (campus central) | -16.4050 | -71.5280 |
| Terminal Terrestre | -16.4290 | -71.5210 |
| Puente Grau | -16.3959 | -71.5390 |
| Aeropuerto A. Rodríguez Ballón | -16.3411 | -71.5831 |
| Cerro Colorado (aprox.) | -16.3670 | -71.5600 |

### 9.2 Elementos en el mapa

| Elemento | Tipo QML |
|---|---|
| Rutas | `MapPolyline` |
| Paraderos y buses | `MapItemView` + `MapQuickItem` |
| Posición del vehículo | `MapQuickItem` con `coordinate` enlazada al ViewModel |

### 9.3 Modo de respaldo

* Activación automática si el plugin `osm` no carga o no hay red.
* Dibuja el mismo modelo en un `Canvas`/`Shape` con una proyección local.
* La conmutación se encapsula detrás de una abstracción de proyección
  (`MapProjection`) para no tocar el resto del sistema.

### 9.4 Riesgo de tiles

Ver §18 (política de uso de OSM, disponibilidad del plugin, caché offline).

---

## 10. Patrones de diseño

### 10.1 Factory Method (creacional) — mínimo obligatorio

* `VehicleFactory::create(VehicleType)` devuelve `std::unique_ptr<Vehicle>`.
* El código cliente no conoce las clases concretas.
* Tipos iniciales: `TrunkBus`, `FeederBus`.

```cpp
auto vehicle = VehicleFactory::create(VehicleType::TrunkBus);
```

### 10.2 Decorator (estructural) — mínimo obligatorio

Permite modificar dinámicamente las condiciones de una ruta sin tocar la clase base.
Estructura objetivo:

```text
IRoute
  └── Route
        └── RouteDecorator
              ├── TrafficDecorator
              └── AccidentDecorator
```

Ejemplo de efecto acumulado:

```text
Ruta normal                 40 km/h
 + Tráfico pesado           20 km/h
 + Accidente                 5 km/h
```

Decoradores deseables: `TrafficDecorator`, `AccidentDecorator`,
(opcional) `RoadClosureDecorator`.

### 10.3 Observer

* Implementado con **señales/slots de Qt** entre ViewModel y Vista.
* Sin acoplar el Modelo a la interfaz.

---

## 11. Características de C++ moderno

### 11.1 Smart pointers

* `std::unique_ptr` — propiedad exclusiva (p. ej. `Fleet` posee sus vehículos).
* `std::shared_ptr` — recursos compartidos (rutas compartidas entre líneas).
* `std::weak_ptr` — observadores sin propiedad (evitar ciclos).
* Prohibido `new`/`delete` manuales salvo justificación documentada.

```cpp
std::vector<std::unique_ptr<Vehicle>> vehicles;
```

### 11.2 Templates

Uso **real**, no decorativo. Candidatos:

* `template<typename T> class Repository` para colecciones de entidades.
* `template<typename T> class StatisticsAccumulator` para medias (tiempo de espera).
* `template<typename T> T clamp(T value, T lo, T hi)` en `utils/Types.h`.
* `template<typename... Args> void Logger::log(Args&&...)` (variádico).

### 11.3 Lambdas

Usos con función real:

* Conexiones Qt (`connect(..., [this]{ ... })`).
* Acciones de botones y callbacks.
* Eventos temporales (p. ej. activar un incidente tras un retraso):

```cpp
QTimer::singleShot(5000, [&]() {
    // activar incidente
});
```

### 11.4 Sobrecarga de operadores

* Como mínimo `Bus::operator++` que representa el ingreso de un pasajero:

```cpp
Bus& Bus::operator++();   // respeta la capacidad máxima
```

* La API principal mantiene `Bus::boardPassenger()`; el operador es una
  demostración idiomática, no la vía principal.
* Se pueden añadir otras sobrecargas si aportan claridad (p. ej. comparación de
  vehículos por prioridad).

### 11.5 Manejo de errores

Situaciones a manejar:

```text
Bus lleno
Ruta inexistente
Paradero inexistente
Índice inválido
Vehículo no disponible
Ruta bloqueada
Datos de simulación inválidos
```

Reglas:

* **No** usar excepciones para control de flujo normal.
* Preferir `std::optional`, un `Result<T>`/`ErrorCode` propio, o `std::expected`
  si el compilador lo soporta.
* Los errores de datos externos (JSON) se validan al cargar y se reportan.

### 11.6 Multithreading

* El `SimulationEngine` se ejecuta en un **hilo de trabajo** (`QThread` con
  `moveToThread`, o `QThreadPool`), nunca en el hilo de la GUI.
* En cada tick el worker publica un **snapshot** del estado; la GUI lo recibe por
  conexión *queued* y solo pinta.
* El dueño del estado mutable es el worker; la GUI solo lee snapshots.
  Sincronización con `QMutex`/`QReadWriteLock` y contadores atómicos cuando aplique.
* La velocidad de simulación se aplica escalando el **delta de tiempo simulado**,
  no la frecuencia real del `QTimer`.
* Detalles de la interfaz:

```cpp
class SimulationEngine : public QObject {
    Q_OBJECT
public slots:
    void start();
    void pause();
    void resume();
    void reset();
    void setSpeedMultiplier(double factor);
signals:
    void stateUpdated(const SimulationState& snapshot);
    void statisticsUpdated(const Statistics& stats);
};
```

---

## 12. Casos de uso / historias de usuario

| ID | Historia de usuario | Requisitos |
|---|---|---|
| HU-01 | Como operador, quiero ver el mapa de Arequipa con las rutas y paraderos para entender la red. | RF-32, RF-33 |
| HU-02 | Como operador, quiero iniciar, pausar y reiniciar la simulación para controlarla. | RF-28 |
| HU-03 | Como analista, quiero acelerar la simulación (hasta 10x) para observar horas de operación en segundos. | RF-29, RF-30 |
| HU-04 | Como operador, quiero ver los buses moviéndose en el mapa en tiempo real. | RF-13, RF-34 |
| HU-05 | Como analista, quiero que la demanda cambie por franja horaria para estudiar horas punta. | RF-10, RF-11 |
| HU-06 | Como operador, quiero que el sistema despache buses automáticamente donde hay más demanda. | RF-18, RF-20, RF-21 |
| HU-07 | Como analista, quiero que un accidente reduzca la velocidad y desvíe rutas. | RF-23, RF-25, RF-26 |
| HU-08 | Como analista, quiero un dashboard con las métricas clave de operación. | RF-38, RF-39 |
| HU-09 | Como docente/estudiante, quiero cargar un escenario y exportar resultados para el informe. | RF-41, RF-43 |
| HU-10 | Como usuario, quiero que la interfaz no se congele mientras la simulación corre. | RNF-02 |

---

## 13. Criterios de aceptación

| ID | Criterio | Vinculado a |
|---|---|---|
| CA-01 | La app arranca y muestra el mapa de Arequipa (o el modo esquemático si no hay red). | RF-32, RF-35 |
| CA-02 | Se observan buses moviéndose entre paraderos y actualizando su ocupación. | RF-13, RF-14 |
| CA-03 | Pausar detiene el avance; reanudar continúa; reiniciar vuelve al estado inicial. | RF-28 |
| CA-04 | Cambiar a 10x avanza la simulación sin congelar la UI. | RF-29, RF-30, RNF-02 |
| CA-05 | El dashboard refleja los cambios en tiempo real. | RF-38, RF-39 |
| CA-06 | Un incidente reduce la velocidad efectiva y se refleja en el mapa. | RF-23, RF-26 |
| CA-07 | El despacho selecciona el paradero de mayor prioridad y un vehículo disponible. | RF-18, RF-21 |
| CA-08 | Dos ejecuciones con la misma semilla producen los mismos resultados. | RNF-03 |
| CA-09 | Se guarda y se vuelve a cargar un escenario sin pérdida de datos. | RF-41 |
| CA-10 | Se exporta un CSV con el histórico de métricas. | RF-43 |
| CA-11 | `ctest` ejecuta la suite de pruebas y todas pasan. | RNF-09 |
| CA-12 | Un bus lleno rechaza pasajeros sin bloquear la simulación. | RF-16 |

---

## 14. Persistencia y exportación

### 14.1 Escenarios (JSON)

Un escenario describe una configuración completa y reproducible:

```json
{
  "name": "Hora punta centro",
  "seed": 12345,
  "durationMinutes": 120,
  "stops": [
    { "id": "S1", "name": "Plaza de Armas", "lat": -16.3989, "lon": -71.5369, "initialWaiting": 12 }
  ],
  "routes": [
    { "id": "R1", "name": "Ruta 1", "stops": ["S1", "S2", "S3"] }
  ],
  "fleet": [
    { "type": "TrunkBus", "count": 4 },
    { "type": "FeederBus", "count": 6 }
  ],
  "demand": { "peakFactor": 3.0, "normalFactor": 1.0, "nightFactor": 0.3 },
  "events": [
    { "type": "accident", "routeId": "R1", "atMinute": 30, "durationMinutes": 15 }
  ]
}
```

* Se carga/guarda con `QJsonDocument`.
* La carga valida el esquema; un archivo inválido produce un error claro sin
  corromper el estado (RF-44).

### 14.2 Exportación de resultados (CSV)

* Serie temporal con columnas como:

```text
simTime,activeBuses,stoppedBuses,waitingPassengers,transportedPassengers,activeIncidents,congestedStops,avgWaitTime
```

* La escritura no debe bloquear el hilo de la GUI.

---

## 15. Pruebas

### 15.1 Unitarias (Qt Test)

| Área | Casos |
|---|---|
| `Route` | agregar/consultar paraderos, índice fuera de rango, longitud, avanzar |
| `Vehicle`/`Bus` | capacidad máxima, `boardPassenger`, `operator++`, bus lleno |
| `DispatchController` | orden de prioridad correcto, selección de vehículo disponible |
| Decoradores | velocidad combinada tráfico + accidente |
| Demanda | generación determinista con semilla fija por franja horaria |
| Persistencia | round-trip JSON, rechazo de escenario inválido |

### 15.2 Integración

* Avanzar el `SimulationEngine` N ticks con semilla fija y comparar el estado final
  (determinismo, RNF-03).

### 15.3 Ejecución

* Integrado en CMake con `enable_testing()` y `ctest`.
* Los tests de dominio no requieren GUI ni red.

---

## 16. Entregables y trazabilidad

### 16.1 Entregables

1. Código fuente compilable (C++20 + Qt 6 + CMake).
2. `requerimientos.md` (este documento) y `README.md`.
3. Escenarios JSON de ejemplo.
4. Suite de tests ejecutable.
5. Guion de demostración.

### 16.2 Trazabilidad requisito → evidencia

| Competencia (OBJ) | Dónde se evidencia | Prototipo |
|---|---|---|
| OBJ-01 POO | Modelo de dominio (§7) | P1–P5 |
| OBJ-02 C++ moderno | Todo el código; `enum class`, `auto`, RAII | P1–P5 |
| OBJ-03 MVC/MVVM | Separación model / viewmodel / qml (§8) | P2–P3 |
| OBJ-04 Patrones | Factory (§10.1), Decorator (§10.2), Observer (§10.3) | P4 |
| OBJ-05 Herencia/polimorfismo | Jerarquía `Vehicle` (§7.2) | P1–P2 |
| OBJ-06 Smart pointers | `Fleet` con `unique_ptr`; `weak_ptr` en observadores | P2 |
| OBJ-07 Templates | `Repository<T>`, `StatisticsAccumulator<T>` (§11.2) | P3–P4 |
| OBJ-08 Lambdas | Conexiones Qt, eventos temporales (§11.3) | P2–P4 |
| OBJ-09 Sobrecarga | `Bus::operator++` (§11.4) | P2 |
| OBJ-10 Errores | Tipos de error en `utils/` (§11.5) | P2–P5 |
| OBJ-11 Multithreading | `SimulationEngine` en `QThread` (§11.6) | P3 |
| OBJ-12 GUI Qt | QML + ViewModels (§8) | P1–P5 |

---

## 17. Plan de 5 prototipos (evolutivo)

Metodología: **prototipado evolutivo**. Cada prototipo es un **programa funcional
completo** que parte del anterior y lo mejora. Cada uno responde una **hipótesis** y
deja una **decisión de diseño** para el siguiente. Desde P2 se mantienen **tests**
para evitar regresiones al refactorizar.

| # | Prototipo | Hipótesis / pregunta | Qué es funcional (demo) | Decisión que habilita |
|---|---|---|---|---|
| **P1** | Base + mapa real | ¿Funciona el pipeline C++↔QML con Qt Location y el mapa real de Arequipa? | App con mapa OSM, Model reducido (`Vehicle`, `Bus`, `Route`, `Stop`, `Fleet`), 1 ruta, 2 paraderos y 1 bus avanzando; fallback esquemático; errores básicos. | Confirma CMake + plugin `osm` + uso de `QGeoCoordinate` (resuelve el riesgo #1). |
| **P2** | Simulador funcional | ¿El modelo de dominio soporta un sistema completo en un solo hilo? | Múltiples rutas/paraderos/buses; pasajeros esperan y abordan (`operator++`); Iniciar/Pausar/Reanudar/Reiniciar y velocidad; reloj y franjas. | Congela las interfaces del Model. |
| **P3** | Concurrencia + Dashboard | ¿La UI se mantiene fluida con el motor en otro hilo y cuánta información cabe en pantalla? | Motor en `QThread` con snapshots; rutas/paraderos/buses animados en `MapView`; `QAbstractListModel`; Dashboard en vivo. | Define el contrato de snapshot/threading y los list models. |
| **P4** | Dinámica + patrones | ¿Factory/Decorator y el despacho por prioridad simplifican o complican el diseño? | Demanda por franja horaria, tráfico e incidentes, `DispatchController` con prioridad, `VehicleFactory`, decoradores, escenarios y log de eventos. | Valida los patrones y la fórmula de prioridad. |
| **P5** | Producto final | ¿Es robusto, reproducible y presentable? | Todo lo anterior + persistencia JSON, exportación CSV, suite completa de tests, errores pulidos, README y guion de demo. | Producto entregable. |

### 17.1 Criterio de éxito por prototipo

* **P1:** el mapa de Arequipa se muestra y un bus recorre una ruta.
* **P2:** la simulación completa avanza, pausa y reinicia, con pasajeros subiendo y bajando.
* **P3:** con 50+ vehículos, la UI sigue respondiendo y el dashboard se actualiza en vivo.
* **P4:** un incidente afecta la velocidad y el despacho responde a la demanda.
* **P5:** `ctest` pasa al 100 %, se carga/guarda un escenario y se exporta el CSV.

---

## 18. Riesgos y mitigaciones

| # | Riesgo | Prob. | Impacto | Mitigación |
|---|---|---|---|---|
| R-01 | El plugin `osm` de Qt Location no está disponible o requiere API key/watermark según la versión. | Media | Alto | **Validar en P1**. Fallback esquemático (§9.3). Documentar versión de Qt. |
| R-02 | Sin Internet en la demo, el mapa queda vacío. | Media | Alto | Caché de tiles + modo offline por defecto configurable. |
| R-03 | Determinismo roto por concurrencia. | Media | Medio | Snapshot único por tick y RNG con semilla fija; test de integración. |
| R-04 | Regresión al refactorizar entre prototipos. | Alta | Medio | Tests desde P2; ejecutar `ctest` antes de cerrar cada prototipo. |
| R-05 | Acoplamiento del Modelo a la Vista o al mapa. | Media | Medio | Revisión de capas (RNF-06); el Modelo solo usa `QGeoCoordinate`, nunca tipos QML. |
| R-06 | Crecimiento desmedido de clases/archivos. | Media | Bajo | Regla explícita: no crear archivos que no aporten funcionalidad (§8.3). |
| R-07 | Ralentización con muchas entidades. | Baja | Medio | Modelos eficientes; medir con 50+ vehículos; evitar refrescos redundantes. |
| R-08 | Uso de tiles de OSM fuera de su política. | Baja | Medio | Uso académico moderado, cacheo y posibilidad de cambiar de proveedor. |

---

## Apéndice A — Resumen de cambios respecto al borrador original

* Se corrigió la numeración de secciones y el documento quedó completo.
* Se resolvió la contradicción **Qt Widgets vs Qt Quick**: la interfaz es **QML**.
* Se reescribió la restricción de "servicios externos" para permitir el **mapa base
  OSM** con **degradación offline** (§4.2, RNF-01).
* Se añadieron las secciones faltantes: **multithreading**, **templates**,
  **persistencia y exportación**, **pruebas** y **entregables/trazabilidad**.
* Se cambió la metodología a **prototipado evolutivo de 5 prototipos** (§17).
* Se reemplazó `QPointF` por **`QGeoCoordinate`** para las posiciones, coherente con
  el mapa real.
* Se fijaron **RF/RNF**, **criterios de aceptación**, **historias de usuario** y
  **matriz de riesgos**.
