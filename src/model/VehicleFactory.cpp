#include "model/VehicleFactory.h"
#include "common/Exceptions.h"

namespace sit {

std::unique_ptr<Vehicle> VehicleFactory::create(VehicleType type, int id, Position pos) {
    switch (type) {
        case VehicleType::Conventional: return std::make_unique<ConventionalBus>(id, pos);
        case VehicleType::Articulated:  return std::make_unique<ArticulatedBus>(id, pos);
        case VehicleType::Feeder:       return std::make_unique<FeederBus>(id, pos);
    }
    throw InvalidVehicleTypeException("desconocido");
}

std::unique_ptr<Vehicle> VehicleFactory::create(const std::string& type, int id, Position pos) {
    if (type == "convencional") return create(VehicleType::Conventional, id, pos);
    if (type == "articulado")   return create(VehicleType::Articulated, id, pos);
    if (type == "alimentador")  return create(VehicleType::Feeder, id, pos);
    throw InvalidVehicleTypeException(type);
}

} // namespace sit
