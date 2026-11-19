#pragma once
#include <cmath>
#include <limits>
#include <ostream>

namespace sit {

// Coordenada geografica (WGS84). Es un tipo trivial para poder viajar dentro de Snapshot
// entre el hilo de simulacion y la interfaz sin depender de Qt.
struct GeoCoordinate {
    double latitude{std::numeric_limits<double>::quiet_NaN()};
    double longitude{std::numeric_limits<double>::quiet_NaN()};

    bool isValid() const {
        return std::isfinite(latitude) && std::isfinite(longitude) && latitude >= -90.0 &&
               latitude <= 90.0 && longitude >= -180.0 && longitude <= 180.0;
    }

    // Distancia aproximada en metros (haversine).
    double distanceTo(const GeoCoordinate& o) const {
        if (!isValid() || !o.isValid()) return 0.0;
        static constexpr double earthRadius = 6371000.0;
        static constexpr double degToRad = 3.14159265358979323846 / 180.0;
        const double dLat = (o.latitude - latitude) * degToRad;
        const double dLon = (o.longitude - longitude) * degToRad;
        const double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
                         std::cos(latitude * degToRad) * std::cos(o.latitude * degToRad) *
                             std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
        return 2.0 * earthRadius * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    }

    bool operator==(const GeoCoordinate& o) const {
        return latitude == o.latitude && longitude == o.longitude;
    }
    bool operator!=(const GeoCoordinate& o) const { return !(*this == o); }
};

inline std::ostream& operator<<(std::ostream& os, const GeoCoordinate& c) {
    return os << "(" << c.latitude << ", " << c.longitude << ")";
}

} // namespace sit
