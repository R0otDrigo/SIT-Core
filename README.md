# TEO-TO — Simulador de flotas de transporte público (SIT Arequipa)

Simulador académico de gestión y optimización de una flota de transporte público,
inspirado en el **SIT de Arequipa**. Escrito en **C++20 + Qt 6 (Qt Quick/QML)** y
construido con **CMake**.

> Es un **simulador**, no un sistema oficial ni una réplica exacta del SIT real.

## Documentación

* [`requerimientos.md`](requerimientos.md) — especificación completa (RF/RNF,
  arquitectura, patrones, criterios de aceptación, plan de 5 prototipos y riesgos).
* [`docs/archive/proyecto-borrador.md`](docs/archive/proyecto-borrador.md) — borrador
  original conservado como referencia.

## Estado actual: Prototipo 1 (P1)

P1 es el primer programa funcional y valida el pipeline C++ ↔ QML con el mapa real:

* Modelo reducido: `Vehicle`, `Bus`, `TrunkBus`, `FeederBus`, `Route`, `Stop`, `Fleet`.
* Mapa real de Arequipa con **OpenStreetMap** (`Qt Location`).
* Una ruta con 4 paraderos y un bus que avanza entre ellos.
* Embarque/desembarque con `Bus::operator++` y control de capacidad.
* **Modo esquemático** de respaldo si el mapa base no está disponible.
* Controles: Iniciar/Pausar, Reiniciar y velocidad (0.5x–10x).

## Requisitos

* CMake ≥ 3.21
* Compilador con C++20
* Qt 6.5 o superior con los módulos: `Core`, `Gui`, `Qml`, `Quick`,
  `QuickControls2`, `Location`, `Positioning`, `Network`.

En Arch/CachyOS:

```bash
sudo pacman -S qt6-quickcontrols2 qt6-location qt6-positioning qt6-network
```

## Compilar y ejecutar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/teo_to
```

## Controles

| Acción | Control |
|---|---|
| Iniciar / Pausar | Botón *Iniciar/Pausar* |
| Reiniciar | Botón *Reiniciar* |
| Velocidad de simulación | Selector 0.5x / 1x / 2x / 5x / 10x |
| Forzar modo esquemático | Interruptor *Esquemático* |

## Estructura

```text
src/
├── main.cpp
├── model/        # entidades puras de dominio
├── viewmodel/    # QObject expuesto a QML
├── utils/        # constantes, errores, logger
└── qml/          # Main.qml, MapView.qml, MapFallback.qml
```

El archivo `teo_to.log` (en el directorio temporal del sistema) recoge los mensajes
de Qt, útil porque Qt no siempre escribe a consola.

## Plan de prototipos

| # | Prototipo | Estado |
|---|---|---|
| P1 | Base + mapa real | ✅ Completado |
| P2 | Simulador funcional (flota, demanda, controles) | ⏳ Pendiente |
| P3 | Concurrencia (`QThread`) + Dashboard | ⏳ Pendiente |
| P4 | Dinámica + patrones (Factory, Decorator, despacho) | ⏳ Pendiente |
| P5 | Persistencia JSON/CSV + tests + pulido | ⏳ Pendiente |

Metodología: **prototipado evolutivo** — cada prototipo mejora al anterior y se
mantiene funcional. Ver `requerimientos.md` §17.
