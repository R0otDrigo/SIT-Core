# TEO-TO / SIT-Core

Simulador académico de una flota de transporte público inspirado en el SIT de Arequipa.
Es el proyecto final de Tecnología de Objetos de la UNSA 2026; **no es un sistema oficial**.

## Estado del proyecto

La aplicación está en el **Prototipo 1 (P1)**: muestra el mapa de Arequipa (OpenStreetMap,
con modo esquemático de respaldo), una ruta con cuatro paraderos y un bus en movimiento.
El desarrollo sigue un enfoque de **prototipado evolutivo** en cinco prototipos.

## Tecnologías y arquitectura

- C++20, Qt 6.5+, Qt Quick/QML y CMake 3.21+.
- Vista en QML y lógica de dominio en C++; el puente reactivo se implementa como ViewModel.
- El modelo usa coordenadas geográficas (`QGeoCoordinate`) para representarse en el mapa.
- La factoría de vehículos (`VehicleFactory`) y el `Repository<T>` genérico están integrados.
- Los demás patrones se incorporarán cuando sus funcionalidades estén implementadas.

## Requisitos

Se necesita un compilador C++20, CMake 3.21+ y Qt 6.5+ con Core, Gui, Qml, Quick,
QuickControls2, Location, Positioning y Network. En Arch/CachyOS:

```bash
sudo pacman -S qt6-quickcontrols2 qt6-location qt6-positioning qt6-network
```

## Compilar, probar y ejecutar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure
./build/src/teo_to
```

## Controles de P1

| Acción | Control |
|---|---|
| Iniciar / pausar | Botón de la barra superior |
| Reiniciar | Botón Reiniciar |
| Velocidad | Selector 0.5x / 1x / 2x / 5x / 10x |
| Cambiar al mapa esquemático | Interruptor Esquemático |

Los mensajes de Qt se escriben en `teo_to.log` dentro del directorio temporal del sistema.

## Estructura

```text
src/model/       Entidades de dominio y factoría de vehículos
src/common/      Repository genérico
src/viewmodel/   Objetos C++ expuestos a QML
src/qml/         Vistas y componentes gráficos
tests/           Pruebas unitarias del modelo y Repository
```

## Prototipos

| Prototipo | Alcance | Estado |
|---|---|---|
| P1 | Mapa, ruta, paraderos y un bus | Completado |
| P2 | Flota, demanda y controles ampliados | Pendiente |
| P3 | Motor concurrente y dashboard | Pendiente |
| P4 | Tráfico, incidentes y despacho | Pendiente |
| P5 | JSON/CSV, pruebas completas y pulido | Pendiente |

Ver [`requerimientos.md`](requerimientos.md) para la especificación y los criterios de
aceptación. El borrador inicial está conservado en
[`docs/archive/proyecto-borrador.md`](docs/archive/proyecto-borrador.md).

## Flujo de trabajo del equipo

- Usar ramas `feature/<nombre>` y Pull Requests hacia `main`, con al menos una revisión.
- Mantener `main` compilable y ejecutar `ctest` antes de solicitar la integración.
- Usar mensajes de commit en formato convencional, por ejemplo `feat: agrega ...`,
  `fix: ...`, `test: ...` y `docs: ...`.

## Integrantes

Retamozo Angel, Castillo Rodrigo, Estefanero Rodrigo, Mamani Renzo, Camani Ronald y
Sarmiento Limberg.
