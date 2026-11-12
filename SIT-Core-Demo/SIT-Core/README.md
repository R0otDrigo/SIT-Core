# SIT-Core — Sistema de Gestión y Monitoreo de Flotas (Demo)

Prototipo en **C++ / Qt 6 (Qt Widgets)** con autenticación Firebase + **Modo Demo offline**,
5 roles con dashboard propio y tema corporativo de flota.

## Compilar y ejecutar

Requisitos: Qt 6.4+ (módulos `Widgets` y `Network`), CMake 3.16+, compilador C++17.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/SIT-Core
```

Con Qt Creator: abrir `CMakeLists.txt` y ejecutar.

> Opcional: `-DSIT_QML_LOADING=ON` usa `cargando.qml` (QQuickWidget) en la pantalla de carga
> (requiere `Qt6::Quick` y `Qt6::QuickWidgets`). Por defecto se usa un QDialog animado en C++.

## Credenciales de la demo (Modo Demo / Mock Auth)

No requieren Firebase ni API Key. **Cualquier contraseña es válida** (o usa los botones
de *Acceso Rápido Demo* de la pantalla de login).

| Correo                    | Rol (`id`)                          | Dashboard                                   |
|---------------------------|-------------------------------------|---------------------------------------------|
| `manager@sit.com`         | `fleet_manager`                     | Administración central de la flota          |
| `conductor@sit.com`       | `user`                              | Panel del conductor                         |
| `trafico@sit.com`         | `jefe_de_trafico`                   | Monitoreo de rutas + delegación IA / RPA    |
| `desplazamiento@sit.com`  | `responsable_de_desplazamientos`    | Horarios, salidas/llegadas y asignaciones   |
| `guest@sit.com`           | `guess`                             | Sandbox con selector para probar los 4 roles|

## Qué probar en la presentación

- **fleet_manager**: métricas, estado de flota, alertas, tabla de unidades; en *Conductores* → **+ Registrar conductor** (muestra "Registrando..." ~1,5 s y agrega la fila).
- **user**: km, horas de manejo, consumo, próxima ruta y estado de la unidad.
- **jefe_de_trafico**: tabla de rutas activas con avance; en *Delegación IA / RPA* → **Aceptar y delegar** (carga + tarea delegada).
- **responsable_de_desplazamientos**: horarios de salidas/llegadas; en *Asignaciones* → **Asignar recurso**.
- **guess**: botones superiores para cambiar de rol sin cerrar sesión.

## Autenticación con Firebase real (opcional)

Los correos que no son de demo se validan contra Firebase Auth (REST, `signInWithPassword`).
Configura la API Key por variable de entorno o con QSettings:

```bash
export SIT_FIREBASE_API_KEY="tu_api_key"
```

El rol se lee del *custom claim* `role` del `idToken` (`fleet_manager`, `user`, `jefe_de_trafico`,
`responsable_de_desplazamientos`, `guess`). Si no existe, se asigna `user`.
Sin API Key, el login real muestra un mensaje claro y los usuarios demo siguen funcionando.

## Estructura

```
CMakeLists.txt  resources.qrc  cargando.qml
main.cpp  mainwindow.{h,cpp,ui}
roles.h  theme.{h,cpp}  uihelpers.{h,cpp}
firebaseauthmanager.{h,cpp}      Firebase REST + Mock Auth
logindialog.{h,cpp}              Login + Acceso Rápido Demo
loadingdialog.{h,cpp}            Pantalla "Cargando..." (~1,5 s)
dashboardbase.{h,cpp}            Sidebar + header comunes
admindashboard.{h,cpp}           fleet_manager
userdashboard.{h,cpp}            user
trafficdashboard.{h,cpp}         jefe_de_trafico
movementdashboard.{h,cpp}        responsable_de_desplazamientos
guestdashboard.{h,cpp}           guess (Sandbox)
```

## Notas de migración desde el repositorio anterior

- `login.cpp/.h/.ui` quedan reemplazados por `logindialog.*` (login construido por código).
- `guestdashboard` ahora es el Sandbox (rol `guess`); `admindashboard` pasa a ser el rol `fleet_manager`.
- `main.qml` ya no se usa (la navegación es 100% Qt Widgets). Si conservas `TecnologiaObjetos_en_US.ts`, puedes volver a enlazarlo con `qt_add_translations`.

## Paleta

Azul corporativo `#1E3A8A` · Acento `#3B82F6` · Fondo `#F3F4F6` · Tarjetas `#FFFFFF` ·
Verde `#10B981` · Ámbar `#F59E0B` · Rojo `#EF4444`.
