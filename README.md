# SIT-Core

Simulador y gestor en tiempo real de una red de transporte publico urbano (rutas troncales y
alimentadoras de Arequipa). Proyecto final de Tecnologia de Objetos, UNSA 2026.

## Stack
C++17, Qt 6 (Widgets), CMake >= 3.16, Git.

## Compilar
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
Si Qt6 no esta instalado, se compilan solo el modelo y los tests.

## Arquitectura (MVC)
- `src/model/`      Modelo: C++ puro, sin Qt.
- `src/controller/` Controladores (simulacion, despacho).
- `src/view/`       Vistas Qt.
- `src/common/`     Utilidades: Position, excepciones, Repository<T>.
- `tests/`          Pruebas unitarias.

## Patrones
Factory Method (VehicleFactory), Adapter (fuentes de rutas), Strategy (despacho), Observer.

## Flujo de trabajo Git
- `main` siempre compila. Ramas `feature/<nombre>`, se integra por Pull Request con 1 revision.
- Commits en imperativo: `feat: agrega ...`, `fix: ...`, `test: ...`, `docs: ...`.

## Integrantes
Retamozo Angel, Castillo  Rodrigo, Estefanero Rodrigo,
Mamani Renzo, Camani Ronald, Sarmiento Limberg.
