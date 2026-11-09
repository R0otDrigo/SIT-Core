#pragma once
#include <memory>
#include <string>
#include "model/Vehicle.h"

namespace sit {

enum class VehicleType { Conventional, Articulated, Feeder };

// Patron creacional: Factory Method.
class VehicleFactory {
public:
    static std::unique_ptr<Vehicle> create(VehicleType type, int id, Position pos = {});
    // Util para cargar desde archivos: "convencional", "articulado", "alimentador".
    static std::unique_ptr<Vehicle> create(const std::string& type, int id, Position pos = {});
};

} // namespace sit
