#pragma once

namespace teoto::constants {

/// Intervalo real del temporizador de simulacion (ms).
inline constexpr int TickIntervalMs = 50;

/// Capacidades y velocidades maximas por defecto (km/h).
inline constexpr int TrunkBusCapacity = 90;
inline constexpr int FeederBusCapacity = 40;
inline constexpr double TrunkBusMaxSpeedKmh = 60.0;
inline constexpr double FeederBusMaxSpeedKmh = 50.0;

/// Centro y encuadre aproximados de Arequipa.
inline constexpr double ArequipaLatitude = -16.3989;
inline constexpr double ArequipaLongitude = -71.5369;
inline constexpr int ArequipaZoom = 13;

} // namespace teoto::constants
