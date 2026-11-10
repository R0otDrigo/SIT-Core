import QtQuick 2.15

QtObject {
    // Paleta de colores principal de SIT-Core
    readonly property color primary: "#1E3A8A"       // Azul oscuro corporativo
    readonly property color secondary: "#3B82F6"     // Azul claro para botones/acciones
    readonly property color background: "#F3F4F6"    // Gris claro para el fondo de dashboards
    readonly property color surface: "#FFFFFF"       // Blanco para tarjetas o paneles

    // Colores de estado (Ej. para el "Loading...")
    readonly property color success: "#10B981"       // Verde (Conectado/Operativo)
    readonly property color warning: "#F59E0B"       // Naranja (Precaución/Retraso)
    readonly property color error: "#EF4444"         // Rojo (Error en Firebase/Desconectado)

    // Texto
    readonly property color textPrimary: "#111827"
    readonly property color textSecondary: "#6B7280"
}