# 🚚 SIT-Core

Sistema de gestión y monitoreo de flotas.

## 🚀 Avances Actuales

- [x] Conexión y autenticación (Login) con base de datos en **Firebase**.
- [x] Renderizado de **dashboards personalizados** dependiendo del tipo de usuario.
- [x] Interfaz de login sencillo de usuarios finalizada.

## 🎨 Mejoras Visuales (UI/UX)

- [ ] Implementar animaciones de estado (*Loading...*) para cada registro y transición.
- [ ] Definir e implementar una paleta de colores institucional acorde al tema de la flota.
- [ ] Diseñar e integrar logos y assets gráficos (Prioridad media: implementar antes de la fase final).

## 📋 Tareas Pendientes y Gestión de Roles

### 1. Refactorización de Usuarios Actuales
Hacer el Dashboard completo de cada tipo de usuario y actualizar la nomenclatura:
- [ ] **`fleet_manager`** (Reemplaza al antiguo `administrador`): Desarrollo del dashboard de administración central.
- [ ] **`user`** (Conductores/Afiliados): Dashboard simplificado enfocado en estadísticas de recorrido de sus unidades.
- [ ] **`guess`** (Invitado): *⚠️ IMPORTANTE: Mantener solo para pruebas de funcionalidad del fleet_manager. ELIMINAR en el pase a producción.*

### 2. Nuevos Roles por Añadir
- [ ] **`jefe_de_trafico`**: Encargado de tareas de supervisión y delegación de tareas. *(Nota: En fase de pruebas se evaluará automatizar esto mediante un RPA o IA).*
- [ ] **`responsable_de_desplazamientos`**: Encargado de velar por los horarios y la correcta asignación de recursos.

---
*Documentación actualizada para el equipo de desarrollo.*




