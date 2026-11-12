#pragma once
#include <cmath>
#include <ostream>

namespace sit {

// Punto 2D del mapa. Demuestra sobrecarga de operadores.
struct Position {
    double x{0.0};
    double y{0.0};

    Position operator+(const Position& o) const { return {x + o.x, y + o.y}; }
    Position operator-(const Position& o) const { return {x - o.x, y - o.y}; }
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Position& o) const { return !(*this == o); }

    double distanceTo(const Position& o) const { return std::hypot(x - o.x, y - o.y); }
};

inline std::ostream& operator<<(std::ostream& os, const Position& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

} // namespace sit
