#pragma once
#include <QString>

// Tema corporativo "Flota".
namespace Theme {

inline const QString Navy   = QStringLiteral("#1E3A8A"); // Sidebar / headers
inline const QString Accent = QStringLiteral("#3B82F6"); // Botones / destacados
inline const QString Bg     = QStringLiteral("#F3F4F6"); // Fondo principal
inline const QString Card   = QStringLiteral("#FFFFFF"); // Tarjetas
inline const QString Green  = QStringLiteral("#10B981"); // En ruta / operativo
inline const QString Amber  = QStringLiteral("#F59E0B"); // Retraso / alerta
inline const QString Red    = QStringLiteral("#EF4444"); // Detenido / fallo
inline const QString Text   = QStringLiteral("#111827");
inline const QString Muted  = QStringLiteral("#6B7280");
inline const QString Border = QStringLiteral("#E5E7EB");

// Hoja de estilos QSS global de la aplicación.
QString styleSheet();

} // namespace Theme
